// preview.cpp — desktop harness for the doodle engine.
// Renders faces to PNG so the style can be iterated without hardware.
//
// build:  clang++ -O2 -std=c++14 -I../../src preview.cpp -lz -o preview
// usage:  ./preview poster <seed> [cols] [rows] [cellW] [out.png]
//         ./preview stick  <seed> [out.png]        (135x240, device-size test)
//         ./preview big    <seed> [px] [out.png]
//         ./preview calib  [n]                     (score histogram for tiers)
//         ./preview anim   <seed> [frames] [out.gif]  (animated device simulation)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include "doodleink.h"

using namespace dd;

struct BufCanvas : Canvas {
  int W, H;
  uint8_t* px;
  BufCanvas(int w, int h) : W(w), H(h) { px = (uint8_t*)malloc((size_t)w * h * 3); memset(px, 255, (size_t)w * h * 3); }
  ~BufCanvas() { free(px); }
  int width() const override { return W; }
  int height() const override { return H; }
  void blend(int x, int y, const uint8_t rgb[3], float a) override {
    if (x < 0 || y < 0 || x >= W || y >= H) return;
    uint8_t* p = px + ((size_t)y * W + x) * 3;
    for (int i = 0; i < 3; i++) p[i] = (uint8_t)(p[i] + (rgb[i] - p[i]) * a + 0.5f);
  }
};

static void put32(FILE* f, uint32_t v) {
  fputc((v >> 24) & 255, f); fputc((v >> 16) & 255, f); fputc((v >> 8) & 255, f); fputc(v & 255, f);
}
static void chunk(FILE* f, const char* tag, const uint8_t* data, uint32_t len) {
  put32(f, len);
  fwrite(tag, 1, 4, f);
  if (len) fwrite(data, 1, len, f);
  uint32_t crc = crc32(0, (const Bytef*)tag, 4);
  if (len) crc = crc32(crc, data, len);
  put32(f, crc);
}
static bool writePNG(const char* path, const BufCanvas& c) {
  FILE* f = fopen(path, "wb");
  if (!f) return false;
  static const uint8_t sig[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
  fwrite(sig, 1, 8, f);
  uint8_t ihdr[13];
  uint32_t w = c.W, h = c.H;
  ihdr[0] = w >> 24; ihdr[1] = w >> 16; ihdr[2] = w >> 8; ihdr[3] = w;
  ihdr[4] = h >> 24; ihdr[5] = h >> 16; ihdr[6] = h >> 8; ihdr[7] = h;
  ihdr[8] = 8; ihdr[9] = 2; ihdr[10] = 0; ihdr[11] = 0; ihdr[12] = 0;
  chunk(f, "IHDR", ihdr, 13);
  size_t raw = (size_t)(c.W * 3 + 1) * c.H;
  uint8_t* rows = (uint8_t*)malloc(raw);
  for (int y = 0; y < c.H; y++) {
    rows[(size_t)y * (c.W * 3 + 1)] = 0;
    memcpy(rows + (size_t)y * (c.W * 3 + 1) + 1, c.px + (size_t)y * c.W * 3, c.W * 3);
  }
  uLongf zlen = compressBound(raw);
  uint8_t* z = (uint8_t*)malloc(zlen);
  compress2(z, &zlen, rows, raw, 6);
  chunk(f, "IDAT", z, (uint32_t)zlen);
  chunk(f, "IEND", nullptr, 0);
  fclose(f);
  free(rows); free(z);
  return true;
}

// ---------------------------------------------------------------- tiny GIF
// Minimal animated GIF writer: global 256-color palette, 9-bit LZW with
// frequent clear codes (no dictionary), infinite loop. Big-ish files, but
// dependency-free and every decoder eats it.
struct GifWriter {
  FILE* f = nullptr;
  int W = 0, H = 0, palN = 0;
  uint8_t pal[256][3];
  uint8_t* lut = nullptr;  // 32768-entry 5:5:5 -> palette index

  void buildPalette(const uint8_t* rgb, size_t px) {
    static uint32_t hist[32768];
    memset(hist, 0, sizeof hist);
    for (size_t i = 0; i < px; i++) {
      int k = ((rgb[i * 3] >> 3) << 10) | ((rgb[i * 3 + 1] >> 3) << 5) | (rgb[i * 3 + 2] >> 3);
      hist[k]++;
    }
    palN = 0;
    for (int n = 0; n < 256; n++) {
      uint32_t best = 0; int bi = -1;
      for (int k = 0; k < 32768; k++)
        if (hist[k] > best) { best = hist[k]; bi = k; }
      if (bi < 0) break;
      hist[bi] = 0;
      pal[palN][0] = ((bi >> 10) & 31) << 3 | 4;
      pal[palN][1] = ((bi >> 5) & 31) << 3 | 4;
      pal[palN][2] = (bi & 31) << 3 | 4;
      palN++;
    }
    lut = (uint8_t*)malloc(32768);
    for (int k = 0; k < 32768; k++) {
      int r = ((k >> 10) & 31) << 3, g = ((k >> 5) & 31) << 3, b = (k & 31) << 3;
      int bd = 1 << 30, bi2 = 0;
      for (int p2 = 0; p2 < palN; p2++) {
        int dr = r - pal[p2][0], dg = g - pal[p2][1], db = b - pal[p2][2];
        int d = dr * dr + dg * dg * 2 + db * db;
        if (d < bd) { bd = d; bi2 = p2; }
      }
      lut[k] = (uint8_t)bi2;
    }
  }

  bool open(const char* path, int w, int h, const uint8_t* paletteFrame) {
    W = w; H = h;
    buildPalette(paletteFrame, (size_t)w * h);
    f = fopen(path, "wb");
    if (!f) return false;
    fwrite("GIF89a", 1, 6, f);
    fputc(W & 255, f); fputc(W >> 8, f); fputc(H & 255, f); fputc(H >> 8, f);
    fputc(0xF7, f); fputc(0, f); fputc(0, f);         // global table, 256 colors
    for (int i = 0; i < 256; i++) {
      const uint8_t* c = i < palN ? pal[i] : pal[0];
      fputc(c[0], f); fputc(c[1], f); fputc(c[2], f);
    }
    static const uint8_t loopExt[] = { 0x21, 0xFF, 0x0B, 'N','E','T','S','C','A','P','E','2','.','0', 3, 1, 0, 0, 0 };
    fwrite(loopExt, 1, sizeof loopExt, f);
    return true;
  }

  void addFrame(const uint8_t* rgb, int delayCS) {
    fputc(0x21, f); fputc(0xF9, f); fputc(4, f); fputc(0x04, f);
    fputc(delayCS & 255, f); fputc(delayCS >> 8, f); fputc(0, f); fputc(0, f);
    fputc(0x2C, f); fputc(0, f); fputc(0, f); fputc(0, f); fputc(0, f);
    fputc(W & 255, f); fputc(W >> 8, f); fputc(H & 255, f); fputc(H >> 8, f); fputc(0, f);
    fputc(8, f);  // LZW min code size
    uint32_t bitbuf = 0; int bitcnt = 0, blen = 0, count = 0;
    uint8_t block[256];
    #define PUTCODE(code) { bitbuf |= (uint32_t)(code) << bitcnt; bitcnt += 9;       while (bitcnt >= 8) { block[blen++] = bitbuf & 0xFF; bitbuf >>= 8; bitcnt -= 8;         if (blen == 255) { fputc(255, f); fwrite(block, 1, 255, f); blen = 0; } } }
    PUTCODE(256)
    size_t n = (size_t)W * H;
    for (size_t i = 0; i < n; i++) {
      int k = ((rgb[i * 3] >> 3) << 10) | ((rgb[i * 3 + 1] >> 3) << 5) | (rgb[i * 3 + 2] >> 3);
      PUTCODE(lut[k])
      if (++count == 240) { PUTCODE(256) count = 0; }
    }
    PUTCODE(257)
    #undef PUTCODE
    if (bitcnt > 0) block[blen++] = bitbuf & 0xFF;
    if (blen) { fputc(blen, f); fwrite(block, 1, blen, f); }
    fputc(0, f);
  }

  void close() { fputc(0x3B, f); fclose(f); free(lut); }
};

static void printCard(const FaceTraits& f) {
  printf("seed %u  score %.2f  tier %s\n", f.seed, f.score, tierName(f.score));
  for (int i = 0; i < C_COUNT; i++)
    printf("  %-12s %-10s %.1f%%\n", CAT_LABELS[i], traitName(f, i), f.pct[i] * 100);
  printf("  pose: yaw %.2f pitch %.2f roll %.2f\n", f.turn, f.pitch, f.roll);
}

int main(int argc, char** argv) {
  const char* mode = argc > 1 ? argv[1] : "poster";
  uint32_t seed = argc > 2 ? (uint32_t)strtoul(argv[2], nullptr, 10) : 12345;

  if (!strcmp(mode, "calib")) {
    int n = argc > 2 ? atoi(argv[2]) : 5000;
    int hist[80] = { 0 };
    FaceTraits f;
    for (int i = 0; i < n; i++) {
      rollFace(f, 1000 + i);
      int b = (int)f.score;
      if (b >= 0 && b < 80) hist[b]++;
    }
    int acc = 0;
    for (int i = 0; i < 80; i++)
      if (hist[i]) { acc += hist[i]; printf("%2d: %5d  cum %.1f%%\n", i, hist[i], 100.0 * acc / n); }
    return 0;
  }

  if (!strcmp(mode, "stick")) {
    BufCanvas cv(135, 240);
    uint8_t* cov = (uint8_t*)calloc(cv.W * cv.H, 1);
    paperBackground(cv, seed);
    FaceTraits f;
    rollFace(f, seed);
    float s = (float)cv.H * 0.30f;
    drawFace(cv, cov, f, cv.W * 0.5f, cv.H * 0.46f, s);
    printCard(f);
    const char* out = argc > 3 ? argv[3] : "stick.png";
    writePNG(out, cv);
    printf("wrote %s\n", out);
    free(cov);
    return 0;
  }

  if (!strcmp(mode, "big")) {
    int px = argc > 3 ? atoi(argv[3]) : 480;
    BufCanvas cv(px, px);
    uint8_t* cov = (uint8_t*)calloc(cv.W * cv.H, 1);
    paperBackground(cv, seed);
    FaceTraits f;
    rollFace(f, seed);
    drawFace(cv, cov, f, px * 0.5f, px * 0.52f, px * 0.3f);
    printCard(f);
    const char* out = argc > 4 ? argv[4] : "big.png";
    writePNG(out, cv);
    printf("wrote %s\n", out);
    free(cov);
    return 0;
  }

  if (!strcmp(mode, "anim")) {  // simulate the live device loop
    int frames = argc > 3 ? atoi(argv[3]) : 64;
    const char* out = argc > 4 ? argv[4] : "anim.gif";
    BufCanvas cv(135, 240);
    uint8_t* cov = (uint8_t*)calloc(cv.W * cv.H, 1);
    uint8_t* paper = (uint8_t*)malloc((size_t)cv.W * cv.H * 3);
    paperBackground(cv, seed);
    memcpy(paper, cv.px, (size_t)cv.W * cv.H * 3);

    FaceTraits base, live;
    rollFace(base, seed);
    printCard(base);
    float yaw = base.turn, pitch = base.pitch, roll = base.roll;
    float yawT = yaw, pitchT = pitch, rollT = roll;
    int expr = base.idx[C_EXPR];
    uint32_t moodSeed = seed;
    Rng ar(seed ^ 0xA11CEu);
    GifWriter gif;
    bool gifOpen = false;

    // contact sheet of the first 8 frames for quick inspection
    BufCanvas sheet(cv.W * 4, cv.H * 2);

    for (int i = 0; i < frames; i++) {
      if (i % 16 == 0) {  // a new glance target
        yawT = ar.rr(-0.65f, 0.65f); pitchT = ar.rr(-0.15f, 0.25f); rollT = ar.rr(-0.08f, 0.08f);
      }
      if (i > 0 && i % 26 == 0) { expr = ar.ri(0, 6); moodSeed = seed + i; }  // mood swing
      bool blink = (i % 21) == 12;
      yaw += (yawT - yaw) * 0.2f; pitch += (pitchT - pitch) * 0.2f; roll += (rollT - roll) * 0.2f;

      live = base;
      applyMood(live, expr, moodSeed);
      live.turn = clampf(yaw + 0.04f * sinf(i * 0.37f), -0.9f, 0.9f);
      live.pitch = pitch + 0.02f * sinf(i * 0.23f);
      live.roll = roll;
      live.gazeX = clampf(yawT * 0.6f + 0.2f * sinf(i * 0.15f), -0.5f, 0.5f);
      if (blink) live.idx[C_EYES] = 2;

      memcpy(cv.px, paper, (size_t)cv.W * cv.H * 3);
      drawFace(cv, cov, live, cv.W * 0.5f, cv.H * 0.46f, cv.H * 0.30f, base.seed * 31u + (i % 3), 1);

      if (!gifOpen) { gif.open(out, cv.W, cv.H, cv.px); gifOpen = true; }
      gif.addFrame(cv.px, 12);  // ~8.3 fps, like the device
      if (i % 3 == 0 && i / 3 < 8) {
        int sx = (i / 3) % 4 * cv.W, sy = (i / 3) / 4 * cv.H;
        for (int y = 0; y < cv.H; y++)
          memcpy(sheet.px + (((size_t)(sy + y) * sheet.W) + sx) * 3, cv.px + (size_t)y * cv.W * 3, cv.W * 3);
      }
    }
    gif.close();
    writePNG("anim_sheet.png", sheet);
    printf("wrote %s (%d frames) + anim_sheet.png\n", out, frames);
    free(cov); free(paper);
    return 0;
  }

  if (!strcmp(mode, "crew")) {  // 4x3 grid filtered to one vibe (by index)
    int vibe = argc > 2 ? atoi(argv[2]) : 2;
    uint32_t sd = argc > 3 ? (uint32_t)strtoul(argv[3], nullptr, 10) : 1;
    const char* out = argc > 4 ? argv[4] : "crew.png";
    int cell = 220;
    BufCanvas cv(4 * cell, 3 * cell);
    uint8_t* cov = (uint8_t*)calloc(cv.W * cv.H, 1);
    paperBackground(cv, sd);
    FaceTraits f;
    for (int i = 0; i < 12; i++) {
      do { rollFace(f, sd++); } while (f.idx[C_VIBE] != vibe);
      drawFace(cv, cov, f, (i % 4 + 0.5f) * cell, (i / 4 + 0.5f) * cell, cell * 0.56f);
    }
    writePNG(out, cv);
    printf("wrote %s (vibe %s)\n", out, T_VIBE[vibe].name);
    free(cov);
    return 0;
  }

  // poster grid
  int cols = argc > 3 ? atoi(argv[3]) : 6;
  int rows = argc > 4 ? atoi(argv[4]) : 8;
  int cell = argc > 5 ? atoi(argv[5]) : 200;
  const char* out = argc > 6 ? argv[6] : "poster.png";
  BufCanvas cv(cols * cell, rows * cell);
  uint8_t* cov = (uint8_t*)calloc(cv.W * cv.H, 1);
  paperBackground(cv, seed);
  FaceTraits f;
  for (int r = 0; r < rows; r++)
    for (int c = 0; c < cols; c++) {
      uint32_t s = seed + r * cols + c;
      rollFace(f, s);
      Rng jr(s ^ 0xC0FFEE);
      float cx = (c + 0.5f) * cell + jr.rr(-0.02f, 0.02f) * cell;
      float cy = (r + 0.5f) * cell + jr.rr(-0.02f, 0.02f) * cell;
      drawFace(cv, cov, f, cx, cy, cell * 0.62f);
    }
  writePNG(out, cv);
  printf("wrote %s (%dx%d)\n", out, cv.W, cv.H);
  free(cov);
  return 0;
}
