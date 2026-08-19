// dd_ink.h — the "pen". Turns clean geometry into hand-drawn ink:
// wobbly strokes with taper + pressure, pen-lift tremor lines, hatch /
// stipple / scribble / pencil fills, ink crumbs and paper bites.
//
// Rendering model: strokes are stamped as soft discs into a shared
// max-coverage buffer, then composited once at uniform alpha — this gives
// clean anti-aliased variable-width lines without overlap darkening,
// on any target (RGB desktop buffer, RGB565 sprite, e-paper grayscale).
#pragma once
#include "dd_core.h"

namespace dd {

// Minimal render target. Implement blend() over your framebuffer.
struct Canvas {
  virtual ~Canvas() {}
  virtual int width() const = 0;
  virtual int height() const = 0;
  // Blend src color (r,g,b 0..255) at alpha a (0..1) over pixel x,y.
  virtual void blend(int x, int y, const uint8_t rgb[3], float a) = 0;
  // Optional fast path: expose a native RGB565 framebuffer so the ink engine
  // can blend with integer math and no virtual call per pixel — roughly
  // halves frame time on MCUs. Return nullptr to use blend().
  virtual uint16_t* fb565() { return nullptr; }
};

struct RGB { uint8_t r, g, b; };
static inline RGB rgb(uint8_t r, uint8_t g, uint8_t b) { RGB c = { r, g, b }; return c; }

struct StrokeOpt {
  float alpha = -1;     // <0: randomized .68...97
  float amp = -1;       // wobble amplitude; <0: w*.5+.9
  float taper = 0.16f;  // fraction of length used to taper the ends
  float over = 0;       // overshoot the endpoints (flick of the wrist)
  bool ghost = false;   // faint wider repeat passes
  bool wedge = false;   // thin->thick (single hair strokes etc.)
};

struct Ink {
  Canvas* cv = nullptr;
  Rng* R = nullptr;          // stroke-texture randomness
  RGB ink = { 42, 40, 36 };
  RGB paper = { 236, 232, 222 };
  float boost = 1.0f;        // global alpha boost (small screens want ~1.1)
  float strokeFlush = 0.78f; // stroke() compositing strength; markers want ~0.95
  float minW = 0.6f;         // minimum stroke half-width floor
  int speed = 0;             // 0 = gallery quality, 1 = animation frame (fewer crumbs/passes)
  // dry-media texture: 0 = wet ink, 1 = crayon skipping over paper tooth.
  // grainy stamps are attenuated by a fixed spatial noise, so repeated
  // strokes reveal the same paper fibers, like real wax on rough paper.
  float grain = 0;
  uint32_t grainSeed = 7;

  float grainAt(int x, int y) const {
    // coarse tooth (2px clumps) mixed with fine speckle
    uint32_t h1 = (uint32_t)((x >> 1) * 0x8DA6B343) ^ (uint32_t)((y >> 1) * 0xD8163841) ^ grainSeed;
    h1 = (h1 ^ (h1 >> 13)) * 0x5bd1e995u;
    uint32_t h2 = (uint32_t)(x * 0x9E3779B1) ^ (uint32_t)(y * 0x85EBCA77) ^ grainSeed;
    h2 = (h2 ^ (h2 >> 13)) * 0x5bd1e995u;
    float n = 0.65f * ((float)((h1 >> 8) & 0xFFFF) / 65535.0f)
            + 0.35f * ((float)((h2 >> 8) & 0xFFFF) / 65535.0f);
    float tooth = n * n * (3 - 2 * n);      // clumpy
    return (1.0f - grain) + grain * tooth;  // grain=0 solid, grain=1 fully toothy
  }

  // 2D placement transform (rotation + translation), applied to all points.
  float tx = 0, ty = 0, rc = 1, rs = 0;
  void setXform(float cx, float cy, float rot) { tx = cx; ty = cy; rc = fcos(rot); rs = fsin(rot); }

  // Optional 2.5D pose warp (cylinder billboard), applied in local coords
  // before placement. A flat drawing turns and nods without reprojecting
  // its geometry: under yaw, x rides a vertical cylinder of radius pwRx
  // (center shifts by sin, silhouette stays put); under pitch, y rides a
  // horizontal one of radius pwRy. Points beyond a radius keep the cos
  // compression only, so margins sway gently instead of jumping. The whole
  // effect fades from local y pwFade0 to pwFade1, keeping a bust's
  // shoulders planted while the head turns above them. Purely geometric:
  // consumes no randomness, so identity is untouched.
  bool pwOn = false;
  float pwYc = 1, pwYs = 0, pwPc = 1, pwPs = 0;
  float pwRx = 1, pwRy = 1, pwFade0 = 0, pwFade1 = 1;
  float pwOffX = 0, pwOffY = 0;
  void setPose(float yaw, float pitch, float rx, float ry, float fade0, float fade1) {
    pwOn = yaw != 0 || pitch != 0;
    pwYc = fcos(yaw); pwYs = fsin(yaw);
    pwPc = fcos(pitch); pwPs = fsin(pitch);
    pwRx = rx; pwRy = ry; pwFade0 = fade0; pwFade1 = fade1;
    // the head carries over the neck into the turn (dd_face shifts its
    // skull by turn*w*0.16 for the same reason): silhouette and margins
    // translate too, which is what sells the rotation
    pwOffX = pwYs * rx * 0.18f;
    pwOffY = pwPs * ry * 0.12f;
  }
  void clearPose() { pwOn = false; }

  Vec2 xf(float x, float y) const {
    if (pwOn) {
      float k = 1.0f - 0.85f * clampf((y - pwFade0) / (pwFade1 - pwFade0), 0, 1);
      float u = x / pwRx, xp;
      if (u > -1 && u < 1) xp = x * pwYc + pwRx * fastSqrt(1 - u * u) * pwYs;
      else xp = x * pwYc;
      float w2 = y / pwRy, yp;
      if (w2 > -1 && w2 < 1) yp = y * pwPc + pwRy * fastSqrt(1 - w2 * w2) * pwPs;
      else yp = y * pwPc;
      x += (xp - x + pwOffX) * k;
      y += (yp - y + pwOffY) * k;
    }
    Vec2 v = { tx + x * rc - y * rs, ty + x * rs + y * rc };
    return v;
  }

  // Clip polygon (device space after set). Stamps outside are skipped.
  Vec2 clipPts[96];
  int clipN = 0;
  float cbx0 = 0, cby0 = 0, cbx1 = 0, cby1 = 0;
  void setClip(const Vec2* p, int n) {
    clipN = n > 96 ? 96 : n;
    for (int i = 0; i < clipN; i++) clipPts[i] = xf(p[i].x, p[i].y);
    bboxOf(clipPts, clipN, cbx0, cby0, cbx1, cby1);
  }
  void clearClip() { clipN = 0; }
  bool clipOK(float x, float y) const {
    if (clipN == 0) return true;
    if (x < cbx0 || x > cbx1 || y < cby0 || y > cby1) return false;  // cheap reject
    return pip(x, y, clipPts, clipN);
  }

  // ---- coverage buffer (max-blend union of soft stamps) ----
  uint8_t* cov = nullptr;
  int cw = 0, ch = 0;
  int dx0 = 0, dy0 = 0, dx1 = -1, dy1 = -1;
  uint16_t* fbFast = nullptr;  // non-null: native RGB565 integer blending
  // per-row dirty spans: a long diagonal stroke's bounding box is mostly
  // empty, so flush() walks only what was actually stamped. Rows beyond
  // SPAN_ROWS (huge desktop canvases) fall back to the full bbox scan.
  static const int SPAN_ROWS = 2048;
  int16_t rowMin[SPAN_ROWS], rowMax[SPAN_ROWS];
  int spanRows = 0;

  bool begin(Canvas* canvas, Rng* rng, uint8_t* covBuf) {
    cv = canvas; R = rng; cov = covBuf;
    cw = cv->width(); ch = cv->height();
    spanRows = ch < SPAN_ROWS ? ch : SPAN_ROWS;
    fbFast = cv->fb565();
    dx0 = 0; dy0 = 0; dx1 = -1; dy1 = -1;
    for (int i = 0; i < spanRows; i++) { rowMin[i] = 32767; rowMax[i] = -1; }
    for (size_t i = 0; i < (size_t)cw * ch; i++) cov[i] = 0;
    return true;
  }

  // integer blend into the native 565 buffer (fast path only)
  inline void px565(int x, int y, int sr, int sg, int sb, int ia) {
    uint16_t& d = fbFast[y * cw + x];
    int dr = (d >> 11) & 31, dg = (d >> 5) & 63, db = d & 31;
    dr += ((sr - dr) * ia) >> 8;
    dg += ((sg - dg) * ia) >> 8;
    db += ((sb - db) * ia) >> 8;
    d = (uint16_t)((dr << 11) | (dg << 5) | db);
  }

  void stamp(float x, float y, float r) {  // device coords; soft disc, max blend
    if (!clipOK(x, y)) return;
    if (r < 0.32f) r = 0.32f;
    int x0 = (int)floorf(x - r - 1), x1 = (int)ceilf(x + r + 1);
    int y0 = (int)floorf(y - r - 1), y1 = (int)ceilf(y + r + 1);
    if (x0 < 0) x0 = 0; if (y0 < 0) y0 = 0;
    if (x1 >= cw) x1 = cw - 1; if (y1 >= ch) y1 = ch - 1;
    if (x0 > x1 || y0 > y1) return;
    if (dx1 < 0) { dx0 = x0; dy0 = y0; dx1 = x1; dy1 = y1; }
    if (x0 < dx0) dx0 = x0; if (y0 < dy0) dy0 = y0;
    if (x1 > dx1) dx1 = x1; if (y1 > dy1) dy1 = y1;
    // sqrt-free coverage: full inside r-0.5, quadratic falloff to r+0.5
    float R2 = (r + 0.5f) * (r + 0.5f);
    float ri = r - 0.5f;
    float Ri2 = ri > 0 ? ri * ri : 0;
    float invW = 255.0f / (R2 - Ri2);
    for (int py = y0; py <= y1; py++) {
      float dy = (float)py + 0.5f - y;
      float dy2 = dy * dy;
      uint8_t* row = cov + py * cw;
      if (py < spanRows) {
        if (x0 < rowMin[py]) rowMin[py] = (int16_t)x0;
        if (x1 > rowMax[py]) rowMax[py] = (int16_t)x1;
      }
      for (int px = x0; px <= x1; px++) {
        float dx = (float)px + 0.5f - x;
        float d2 = dx * dx + dy2;
        if (d2 >= R2) continue;
        float c = d2 <= Ri2 ? 255.0f : (R2 - d2) * invW;
        if (grain > 0) c *= grainAt(px, py);
        uint8_t v = (uint8_t)c;
        if (v > row[px]) row[px] = v;
      }
    }
  }

  // Stamp along a segment with lerped radius (fills gaps between samples).
  void stampSeg(float x0, float y0, float x1, float y1, float r0, float r1) {
    float d = hypotf2(x1 - x0, y1 - y0);
    float rm = (r0 < r1 ? r0 : r1);
    float step = rm * 0.7f; if (step < 0.4f) step = 0.4f;
    int n = (int)(d / step) + 1;
    for (int i = 0; i <= n; i++) {
      float t = (float)i / (float)n;
      stamp(lerpf(x0, x1, t), lerpf(y0, y1, t), lerpf(r0, r1, t));
    }
  }

  void flush(RGB c, float alpha) {  // composite coverage once, then clear it
    alpha = clampf(alpha * boost, 0, 1);
    if (fbFast) {
      int a256 = (int)(alpha * 256.0f + 0.5f);
      int sr = c.r >> 3, sg = c.g >> 2, sb = c.b >> 3;
      for (int y = dy0; y <= dy1; y++) {
        int sx = y < spanRows ? rowMin[y] : dx0;
        int ex = y < spanRows ? rowMax[y] : dx1;
        if (ex < sx) continue;
        uint8_t* row = cov + y * cw;
        for (int x = sx; x <= ex; x++) {
          if (row[x]) {
            px565(x, y, sr, sg, sb, (a256 * row[x]) >> 8);
            row[x] = 0;
          }
        }
        if (y < spanRows) { rowMin[y] = 32767; rowMax[y] = -1; }
      }
    } else {
      uint8_t col[3] = { c.r, c.g, c.b };
      for (int y = dy0; y <= dy1; y++) {
        int sx = y < spanRows ? rowMin[y] : dx0;
        int ex = y < spanRows ? rowMax[y] : dx1;
        if (ex < sx) continue;
        uint8_t* row = cov + y * cw;
        for (int x = sx; x <= ex; x++) {
          if (row[x]) {
            cv->blend(x, y, col, alpha * (float)row[x] * (1.0f / 255.0f));
            row[x] = 0;
          }
        }
        if (y < spanRows) { rowMin[y] = 32767; rowMax[y] = -1; }
      }
    }
    dx1 = -1; dy1 = -1;
  }

  // Direct one-off soft dot (crumbs, freckles, stipple).
  void dot(float lx, float ly, float r, RGB c, float alpha) {
    Vec2 d = xf(lx, ly);
    if (!clipOK(d.x, d.y)) return;
    alpha = clampf(alpha * boost, 0, 1);
    uint8_t col[3] = { c.r, c.g, c.b };
    int sr = c.r >> 3, sg = c.g >> 2, sb = c.b >> 3;
    int a256 = (int)(alpha * 256.0f + 0.5f);
    int x0 = (int)floorf(d.x - r - 1), x1 = (int)ceilf(d.x + r + 1);
    int y0 = (int)floorf(d.y - r - 1), y1 = (int)ceilf(d.y + r + 1);
    if (x0 < 0) x0 = 0; if (y0 < 0) y0 = 0;
    if (x1 >= cw) x1 = cw - 1; if (y1 >= ch) y1 = ch - 1;
    for (int py = y0; py <= y1; py++)
      for (int px = x0; px <= x1; px++) {
        float dd = r + 0.5f - hypotf2((float)px + 0.5f - d.x, (float)py + 0.5f - d.y);
        if (dd <= 0) continue;
        if (dd > 1) dd = 1;
        if (grain > 0) dd *= grainAt(px, py);
        if (fbFast) px565(px, py, sr, sg, sb, (int)(a256 * dd));
        else cv->blend(px, py, col, alpha * dd);
      }
  }

  // ---- the pencil stroke: wobble + pressure + taper + crumbs ----
  void stroke(const Vec2* pts, int n, float w, const StrokeOpt& o = StrokeOpt()) {
    if (n < 2 || !R) return;
    Rng& r = *R;
    float alpha = o.alpha >= 0 ? o.alpha : r.rr(0.68f, 0.97f);
    float amp = o.amp >= 0 ? o.amp : (w * 0.35f + 0.7f);

    Vec2 work[96];
    int wn = n > 96 ? 96 : n;
    for (int i = 0; i < wn; i++) work[i] = pts[i];
    if (o.over > 0 && wn >= 2) {  // overshoots veer off-axis
      Vec2 a = work[0], b = work[1];
      float d0 = hypotf2(b.x - a.x, b.y - a.y); if (d0 < 1e-4f) d0 = 1;
      float f0 = o.over * r.rr(-0.5f, 0.5f);
      work[0].x = a.x - (b.x - a.x) / d0 * o.over - (b.y - a.y) / d0 * f0;
      work[0].y = a.y - (b.y - a.y) / d0 * o.over + (b.x - a.x) / d0 * f0;
      Vec2 y2 = work[wn - 1], z = work[wn - 2];
      float d1 = hypotf2(y2.x - z.x, y2.y - z.y); if (d1 < 1e-4f) d1 = 1;
      float f1 = o.over * r.rr(-0.5f, 0.5f);
      work[wn - 1].x = y2.x + (y2.x - z.x) / d1 * o.over - (y2.y - z.y) / d1 * f1;
      work[wn - 1].y = y2.y + (y2.y - z.y) / d1 * o.over + (y2.x - z.x) / d1 * f1;
    }

    float plen = 0;
    for (int i = 1; i < wn; i++) plen += hypotf2(work[i].x - work[i - 1].x, work[i].y - work[i - 1].y);
    P96 rsm;
    float step = w * 0.9f; if (step < 2.2f) step = 2.2f;
    if (plen / step > 90) step = plen / 90;   // never truncate a long path
    resample(rsm, work, wn, step);
    int m = rsm.n;
    if (m < 3) { sline(work, wn, w, alpha); return; }

    float p1 = r.rr(0, 7), p2 = r.rr(0, 7), p3 = r.rr(0, 7), p4 = r.rr(0, 7);
    float f1 = r.rr(1.5f, 3.5f), f2 = r.rr(5, 9), f3 = r.rr(11, 17);

    Vec2 prevD = { 0, 0 };
    float prevR = 0;
    Vec2 crumbC[96]; Vec2 crumbN[96]; float crumbH[96]; int crumbCount = 0;

    for (int i = 0; i < m; i++) {
      float t = (float)i / (float)(m - 1);
      Vec2 a = rsm.p[i > 0 ? i - 1 : 0], b = rsm.p[i < m - 1 ? i + 1 : m - 1];
      float nx = -(b.y - a.y), ny = b.x - a.x;
      float d = hypotf2(nx, ny); if (d < 1e-5f) d = 1;
      nx /= d; ny /= d;
      float off = amp * (0.55f * fsin(t * f1 * 2 + p1) + 0.3f * fsin(t * f2 + p2) + 0.15f * fsin(t * f3 + p3));
      float px = rsm.p[i].x + nx * off + r.rr(-0.28f, 0.28f);
      float py = rsm.p[i].y + ny * off + r.rr(-0.28f, 0.28f);
      float tt = t < 1 - t ? t : 1 - t;
      float env = o.wedge ? (0.25f + 0.95f * t) : (0.42f + 0.58f * smoothf(tt / o.taper));
      float half = w * 0.5f * env * (1 + 0.26f * fsin(t * 7.3f + p4) + 0.1f * fsin(t * 19 + p2)) * r.rr(0.9f, 1.12f);
      if (half < 0.28f) half = 0.28f;
      if (half < minW * 0.5f) half = minW * 0.5f;
      Vec2 dev = xf(px, py);
      if (i > 0) stampSeg(prevD.x, prevD.y, dev.x, dev.y, prevR, half);
      prevD = dev; prevR = half;
      if (crumbCount < 96) { crumbC[crumbCount] = dev; crumbN[crumbCount].x = nx * rc - ny * rs; crumbN[crumbCount].y = nx * rs + ny * rc; crumbH[crumbCount] = half; crumbCount++; }
    }
    flush(ink, alpha * strokeFlush);

    // dry granulation: ink crumbs off the edges, paper biting back in
    if (w >= 1.2f) {
      uint8_t icol[3] = { ink.r, ink.g, ink.b };
      uint8_t pcol[3] = { paper.r, paper.g, paper.b };
      int cstep = speed ? 2 : 1;
      for (int i = 0; i < crumbCount; i += cstep) {
        float half = crumbH[i];
        int nd = (int)(half * 1.5f + 0.5f); if (nd < 1) nd = 1; if (nd > 4) nd = 4;
        for (int k = 0; k < nd; k++) {
          if (r.chance(0.3f)) continue;
          float u = r.rr(-1.05f, 1.05f);
          float sz = r.rr(0.35f, 0.75f) + (half > 2 ? 0.2f : 0);
          float cxp = crumbC[i].x + crumbN[i].x * half * u + r.rr(-0.7f, 0.7f);
          float cyp = crumbC[i].y + crumbN[i].y * half * u + r.rr(-0.7f, 0.7f);
          softDotDevice(cxp, cyp, sz, icol, alpha * r.rr(0.2f, 0.55f));
        }
        if (!speed && r.chance(0.45f)) {
          float u = (r.chance(0.5f) ? 1 : -1) * r.rr(0.8f, 1.15f);
          float sz = r.rr(0.45f, 1.0f);
          softDotDevice(crumbC[i].x + crumbN[i].x * half * u, crumbC[i].y + crumbN[i].y * half * u,
                        sz, pcol, r.rr(0.4f, 0.8f));
        }
      }
    }
    if (o.ghost && !speed && r.chance(0.35f)) {
      StrokeOpt g; g.alpha = alpha * 0.14f; g.amp = amp * 1.9f; g.taper = o.taper;
      stroke(pts, n, w * 0.45f, g);
    }
  }

  void softDotDevice(float x, float y, float rad, const uint8_t col[3], float alpha) {
    if (!clipOK(x, y)) return;
    alpha = clampf(alpha * boost, 0, 1);
    int sr = col[0] >> 3, sg = col[1] >> 2, sb = col[2] >> 3;
    int a256 = (int)(alpha * 256.0f + 0.5f);
    int x0 = (int)floorf(x - rad - 1), x1 = (int)ceilf(x + rad + 1);
    int y0 = (int)floorf(y - rad - 1), y1 = (int)ceilf(y + rad + 1);
    if (x0 < 0) x0 = 0; if (y0 < 0) y0 = 0;
    if (x1 >= cw) x1 = cw - 1; if (y1 >= ch) y1 = ch - 1;
    for (int py = y0; py <= y1; py++)
      for (int px = x0; px <= x1; px++) {
        float c = rad + 0.5f - hypotf2((float)px + 0.5f - x, (float)py + 0.5f - y);
        if (c <= 0) continue;
        if (c > 1) c = 1;
        if (fbFast) px565(px, py, sr, sg, sb, (int)(a256 * c));
        else cv->blend(px, py, col, alpha * c);
      }
  }

  // Broken contour: 2-3 overlapping segments, varied width & pressure.
  void broken(const Vec2* pts, int n, float w, const StrokeOpt& o = StrokeOpt()) {
    if (n < 10) { stroke(pts, n, w, o); return; }
    Rng& r = *R;
    int segs = r.ri(2, 3);
    for (int i = 0; i < segs; i++) {
      int a = (int)((float)n * i / segs - (float)n * 0.05f); if (a < 0) a = 0;
      int b = (int)((float)n * (i + 1) / segs + (float)n * 0.09f); if (b > n) b = n;
      StrokeOpt so = o;
      so.alpha = (o.alpha >= 0 ? o.alpha : r.rr(0.72f, 0.97f)) * r.rr(0.85f, 1.05f);
      so.over = (i == 0 || i == segs - 1) ? o.over : w * r.rr(0, 2);
      stroke(pts + a, b - a, w * r.rr(0.8f, 1.2f), so);
    }
  }

  // Thin fine-liner with tremor and occasional pen lifts.
  void sline(const Vec2* pts, int n, float w, float alpha, const RGB* colorOverride = nullptr) {
    if (n < 2 || !R) return;
    Rng& r = *R;
    float plen = 0;
    for (int i = 1; i < n; i++) plen += hypotf2(pts[i].x - pts[i - 1].x, pts[i].y - pts[i - 1].y);
    float rstep = 3.0f;
    if (plen / rstep > 90) rstep = plen / 90;  // never truncate a long path
    P96 rsm;
    resample(rsm, pts, n, rstep);
    int m = rsm.n;
    float p1 = r.rr(0, 7), p2 = r.rr(0, 7), f = r.rr(4, 9);
    float lw = w * r.rr(0.75f, 1.3f) * 0.5f;
    if (lw < minW * 0.5f) lw = minW * 0.5f;
    bool lift = false;
    Vec2 prev = { 0, 0 };
    bool hasPrev = false;
    for (int i = 0; i < m; i++) {
      float t = (float)i / (float)(m - 1 > 0 ? m - 1 : 1);
      Vec2 a = rsm.p[i > 0 ? i - 1 : 0], b = rsm.p[i < m - 1 ? i + 1 : m - 1];
      float nx = -(b.y - a.y), ny = b.x - a.x;
      float d = hypotf2(nx, ny); if (d < 1e-5f) d = 1;
      float off = (w * 0.45f + 0.4f) * (0.6f * fsin(t * f + p1) + 0.4f * fsin(t * f * 2.7f + p2));
      float x = rsm.p[i].x + nx / d * off + r.rr(-0.35f, 0.35f);
      float y = rsm.p[i].y + ny / d * off + r.rr(-0.35f, 0.35f);
      Vec2 dev = xf(x, y);
      if (hasPrev && !lift) stampSeg(prev.x, prev.y, dev.x, dev.y, lw, lw);
      else stamp(dev.x, dev.y, lw);
      prev = dev; hasPrev = true;
      lift = r.chance(0.035f);
    }
    RGB c = colorOverride ? *colorOverride : ink;
    float fa = colorOverride ? alpha : clampf(alpha * 1.3f, 0, 1);
    flush(c, fa);
    if (!colorOverride && w >= 1.3f && !speed) {
      uint8_t icol[3] = { ink.r, ink.g, ink.b };
      uint8_t pcol[3] = { paper.r, paper.g, paper.b };
      for (int i = 0; i < m; i += 2) {
        if (r.chance(0.55f)) continue;
        float sz = r.rr(0.3f, 0.6f);
        Vec2 dev = xf(rsm.p[i].x, rsm.p[i].y);
        float jx = r.rr(-w * 0.8f - 0.6f, w * 0.8f + 0.6f), jy = r.rr(-w * 0.8f - 0.6f, w * 0.8f + 0.6f);
        if (r.chance(0.7f)) softDotDevice(dev.x + jx, dev.y + jy, sz, icol, alpha * r.rr(0.2f, 0.4f));
        else softDotDevice(dev.x + jx, dev.y + jy, sz, pcol, r.rr(0.4f, 0.7f));
      }
    }
  }

  // ---- fills (clip = the polygon itself, tested per stamp) ----

  void hatchFill(const Vec2* poly, int n, float spacing, float ang, float alpha, float w = 1.1f) {
    Rng& r = *R;
    float x0, y0, x1, y1; bboxOf(poly, n, x0, y0, x1, y1);
    float diag = hypotf2(x1 - x0, y1 - y0);
    float cx = (x0 + x1) * 0.5f, cy = (y0 + y1) * 0.5f;
    setClip(poly, n);
    float px = fcos(ang + TAU * 0.25f), py = fsin(ang + TAU * 0.25f);
    float dx = fcos(ang), dy = fsin(ang);
    int cnt = (int)(diag / spacing) + 1;
    for (int i = -cnt; i <= cnt; i++) {
      float t = i * spacing + r.rr(-0.2f, 0.2f) * spacing;
      Vec2 seg[2] = { { cx + px * t - dx * diag * 0.6f, cy + py * t - dy * diag * 0.6f },
                      { cx + px * t + dx * diag * 0.6f, cy + py * t + dy * diag * 0.6f } };
      sline(seg, 2, w, alpha * r.rr(0.6f, 1.1f));
    }
    clearClip();
  }

  void stippleFill(const Vec2* poly, int n, float spacing, float alpha) {
    Rng& r = *R;
    float x0, y0, x1, y1; bboxOf(poly, n, x0, y0, x1, y1);
    int cnt = (int)((x1 - x0) * (y1 - y0) / (spacing * spacing));
    if (speed) cnt = cnt * 2 / 3;
    for (int i = 0; i < cnt; i++) {
      float x = r.rr(x0, x1), y = r.rr(y0, y1);
      float sz = r.rr(0.35f, 0.8f);
      float a = alpha * r.rr(0.5f, 1.2f);
      if (!pip(x, y, poly, n)) continue;
      dot(x, y, sz, ink, a);
    }
  }

  void scribbleFill(const Vec2* poly, int n, float spacing, float alpha) {
    Rng& r = *R;
    float x0, y0, x1, y1; bboxOf(poly, n, x0, y0, x1, y1);
    setClip(poly, n);
    float slope = r.rr(-0.25f, 0.25f);
    for (float y = y0 - spacing; y < y1 + spacing; y += spacing * r.rr(0.7f, 1.3f)) {
      if (r.chance(0.12f)) continue;  // the hand skips a row here and there
      P48 line;
      float ph = r.rr(0, 7), am = spacing * r.rr(0.28f, 0.6f), sl = slope + r.rr(-0.12f, 0.12f);
      for (float x = x0; x <= x1; x += 5)
        line.add(x, y + (x - x0) * sl + fsin(x * 0.55f + ph) * am + r.rr(-1, 1));
      if (line.n > 1) sline(line.p, line.n, 1, alpha * r.rr(0.6f, 1.05f));
    }
    clearClip();
  }

  // Exact scanline polygon fill with ragged edges + alpha dither.
  void scanFill(const Vec2* polyLocal, int n, RGB c, float alpha, float ragged = 0.8f, float dither = 0.18f) {
    Rng& r = *R;
    Vec2 poly[96];
    int pn = n > 96 ? 96 : n;
    for (int i = 0; i < pn; i++) poly[i] = xf(polyLocal[i].x, polyLocal[i].y);
    float x0, y0, x1, y1; bboxOf(poly, pn, x0, y0, x1, y1);
    int iy0 = (int)floorf(y0), iy1 = (int)ceilf(y1);
    if (iy0 < 0) iy0 = 0; if (iy1 >= ch) iy1 = ch - 1;
    uint8_t col[3] = { c.r, c.g, c.b };
    float a = clampf(alpha * boost, 0, 1);
    int sr = c.r >> 3, sg = c.g >> 2, sb = c.b >> 3;
    int a256 = (int)(a * 256.0f + 0.5f);
    for (int py = iy0; py <= iy1; py++) {
      float fy = (float)py + 0.5f + r.rr(-0.3f, 0.3f);
      float xs[16]; int xn = 0;
      for (int i = 0, j = pn - 1; i < pn; j = i++) {
        float ya = poly[i].y, yb = poly[j].y;
        if ((ya > fy) == (yb > fy)) continue;
        float x = poly[i].x + (fy - ya) / (yb - ya) * (poly[j].x - poly[i].x);
        if (xn < 16) xs[xn++] = x;
      }
      // insertion sort
      for (int i = 1; i < xn; i++) { float v = xs[i]; int j = i - 1; while (j >= 0 && xs[j] > v) { xs[j + 1] = xs[j]; j--; } xs[j + 1] = v; }
      for (int i = 0; i + 1 < xn; i += 2) {
        int sx = (int)floorf(xs[i] + r.rr(-ragged, ragged));
        int ex = (int)ceilf(xs[i + 1] + r.rr(-ragged, ragged));
        if (sx < 0) sx = 0; if (ex >= cw) ex = cw - 1;
        if (fbFast) {
          for (int px = sx; px <= ex; px++) {
            int ia = (int)(a256 * (1.0f + r.rr(-dither, dither)));
            px565(px, py, sr, sg, sb, ia > 256 ? 256 : ia);
          }
        } else {
          for (int px = sx; px <= ex; px++) {
            float da = a * (1.0f + r.rr(-dither, dither));
            cv->blend(px, py, col, clampf(da, 0, 1));
          }
        }
      }
    }
  }

  // Solid "pencilled in" dark fill: base + directional scribble passes.
  void pencilFill(const Vec2* poly, int n, float darkness, float sizeHint) {
    Rng& r = *R;
    scanFill(poly, n, ink, darkness * 0.82f, 0.9f, 0.1f);
    setClip(poly, n);
    float x0, y0, x1, y1; bboxOf(poly, n, x0, y0, x1, y1);
    for (int pass = 0; pass < (speed ? 1 : 2); pass++) {
      float slope = r.rr(-0.5f, 0.5f);
      float sp = sizeHint * r.rr(0.045f, 0.065f); if (sp < 1.6f) sp = 1.6f;
      for (float y = y0 - sp; y < y1 + sp; y += sp * r.rr(0.85f, 1.2f)) {
        P48 line;
        float ph = r.rr(0, 7);
        for (float x = x0; x <= x1; x += 5)
          line.add(x, y + (x - x0) * slope + fsin(x * 0.5f + ph) * sp * 0.35f + r.rr(-0.8f, 0.8f));
        if (line.n > 1) sline(line.p, line.n, 1.2f, darkness * r.rr(0.45f, 0.7f));
      }
    }
    clearClip();
  }
};

} // namespace dd
