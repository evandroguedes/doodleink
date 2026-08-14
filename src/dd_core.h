// dd_core.h — math, RNG, small path buffers for the doodle engine.
// Portable C++11, no heap, no OS deps. Works on ESP32 and desktop.
#pragma once
#include <stdint.h>
#include <math.h>

namespace dd {

static const float TAU = 6.28318530718f;

struct Vec2 { float x, y; };

static inline float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
static inline float lerpf(float a, float b, float t) { return a + (b - a) * t; }
static inline float smoothf(float v) { return v <= 0 ? 0 : (v >= 1 ? 1 : v * v * (3 - 2 * v)); }
// Fast sqrt via inverse-sqrt bit trick + two Newton steps (~1e-4 relative
// error — invisible in AA coverage). The ESP32 FPU has no hardware sqrt;
// this cuts the per-pixel stamping cost roughly in half.
static inline float fastSqrt(float x) {
  if (x <= 1.17549435e-38f) return 0;
  union { float f; uint32_t i; } u;
  u.f = x;
  u.i = 0x5f3759dfu - (u.i >> 1);
  float y = u.f, xh = 0.5f * x;
  y = y * (1.5f - xh * y * y);
  y = y * (1.5f - xh * y * y);
  return x * y;
}
static inline float hypotf2(float x, float y) { return fastSqrt(x * x + y * y); }

// Fast sine/cosine (parabolic approx + refinement, ~0.1% error) — newlib's
// software sinf costs hundreds of cycles on the ESP32 and the wobble engine
// calls it constantly. Plenty accurate for hand-drawn geometry.
static inline float fsin(float x) {
  x -= 6.28318531f * floorf(x * 0.15915494f + 0.5f);
  float y = 1.27323954f * x - 0.405284735f * x * fabsf(x);
  return 0.225f * (y * fabsf(y) - y) + y;
}
static inline float fcos(float x) { return fsin(x + 1.57079633f); }

// mulberry32 — same generator as the JS references, so the "feel" of the
// randomness (and any tuned constants) carries over.
struct Rng {
  uint32_t a;
  explicit Rng(uint32_t seed = 1) : a(seed) {}
  float next() {
    a += 0x6D2B79F5u;
    uint32_t t = a;
    t = (t ^ (t >> 15)) * (t | 1u);
    t ^= t + ((t ^ (t >> 7)) * (t | 61u));
    t = t ^ (t >> 14);
    return (float)(t >> 8) * (1.0f / 16777216.0f);
  }
  float rr(float lo, float hi) { return lo + next() * (hi - lo); }
  int ri(int lo, int hi) { int v = lo + (int)(next() * (float)(hi - lo + 1)); return v > hi ? hi : v; }
  bool chance(float p) { return next() < p; }
};

// Weighted trait pick. Returns index, writes probability of the pick.
struct WOpt { const char* name; uint16_t w; };

static inline int pickW(Rng& R, const WOpt* o, int n, float* pct = nullptr) {
  int total = 0;
  for (int i = 0; i < n; i++) total += o[i].w;
  float r = R.next() * (float)total;
  int acc = 0;
  for (int i = 0; i < n; i++) {
    acc += o[i].w;
    if (r < (float)acc || i == n - 1) {
      if (pct) *pct = (float)o[i].w / (float)total;
      return i;
    }
  }
  return 0;
}

// Fixed-capacity point buffer (stack friendly; no heap on device).
template <int CAP>
struct Path {
  Vec2 p[CAP];
  int n = 0;
  void clear() { n = 0; }
  void add(float x, float y) { if (n < CAP) { p[n].x = x; p[n].y = y; n++; } }
  void add(Vec2 v) { add(v.x, v.y); }
};

typedef Path<96> P96;
typedef Path<48> P48;

// Chaikin corner cutting — the secret to hand-drawn-looking curves from
// few keypoints. One pass keeps a hint of the underlying polygon.
template <int CAP>
static inline void chaikin(Path<CAP>& out, const Vec2* in, int n, bool closed, int iters) {
  Vec2 buf[CAP];
  int bn = n;
  for (int i = 0; i < n && i < CAP; i++) buf[i] = in[i];
  for (int it = 0; it < iters; it++) {
    Vec2 tmp[CAP];
    int tn = 0;
    int last = closed ? bn : bn - 1;
    if (!closed && tn < CAP) tmp[tn++] = buf[0];
    for (int i = 0; i < last; i++) {
      const Vec2& a = buf[i];
      const Vec2& b = buf[(i + 1) % bn];
      if (tn + 2 > CAP) break;
      tmp[tn].x = a.x * 0.75f + b.x * 0.25f; tmp[tn].y = a.y * 0.75f + b.y * 0.25f; tn++;
      tmp[tn].x = a.x * 0.25f + b.x * 0.75f; tmp[tn].y = a.y * 0.25f + b.y * 0.75f; tn++;
    }
    if (!closed && tn < CAP) tmp[tn++] = buf[bn - 1];
    for (int i = 0; i < tn; i++) buf[i] = tmp[i];
    bn = tn;
  }
  out.clear();
  for (int i = 0; i < bn; i++) out.add(buf[i]);
}

// Resample a polyline to (roughly) equidistant points.
template <int CAP>
static inline void resample(Path<CAP>& out, const Vec2* in, int n, float step) {
  out.clear();
  if (n < 2) { for (int i = 0; i < n; i++) out.add(in[i]); return; }
  out.add(in[0]);
  float carry = 0;
  for (int i = 0; i < n - 1; i++) {
    float dx = in[i + 1].x - in[i].x, dy = in[i + 1].y - in[i].y;
    float seg = hypotf2(dx, dy);
    if (seg <= 1e-6f) continue;
    float t = step - carry;
    while (t <= seg) {
      out.add(in[i].x + dx * (t / seg), in[i].y + dy * (t / seg));
      t += step;
    }
    carry = seg - (t - step);
  }
  out.add(in[n - 1]);
}

static inline void ellipsePts(P96& out, float cx, float cy, float rx, float ry, int n = 22, float rot = 0) {
  out.clear();
  float cr = fcos(rot), sr = fsin(rot);
  for (int i = 0; i < n; i++) {
    float t = (float)i / (float)n * TAU;
    float x = fcos(t) * rx, y = fsin(t) * ry;
    out.add(cx + x * cr - y * sr, cy + x * sr + y * cr);
  }
}

static inline void arcPts(P96& out, float cx, float cy, float rx, float ry, float a0, float a1, int n = 16) {
  out.clear();
  for (int i = 0; i < n; i++) {
    float t = a0 + (a1 - a0) * (float)i / (float)(n - 1);
    out.add(cx + fcos(t) * rx, cy + fsin(t) * ry);
  }
}

// Organic blob (radius modulated ellipse), chaikin-smoothed.
static inline void blobPts(P96& out, Rng& R, float cx, float cy, float rx, float ry, float rough = 0.17f) {
  // note: no whole-ellipse rotation — elongated blobs must keep their axes
  float ph = R.rr(0, 7);
  Vec2 raw[16];
  for (int i = 0; i < 16; i++) {
    float t = (float)i / 16.0f * TAU;
    float m = 1 + rough * fsin(t * 2 + ph) + rough * 0.6f * fsin(t * 5 + ph * 2.3f);
    raw[i].x = cx + fcos(t) * rx * m;
    raw[i].y = cy + fsin(t) * ry * m;
  }
  chaikin(out, raw, 16, true, 1);
}

// Even-odd point-in-polygon.
static inline bool pip(float x, float y, const Vec2* p, int n) {
  bool in = false;
  for (int i = 0, j = n - 1; i < n; j = i++) {
    if (((p[i].y > y) != (p[j].y > y)) &&
        (x < (p[j].x - p[i].x) * (y - p[i].y) / (p[j].y - p[i].y) + p[i].x))
      in = !in;
  }
  return in;
}

static inline void bboxOf(const Vec2* p, int n, float& x0, float& y0, float& x1, float& y1) {
  x0 = y0 = 1e9f; x1 = y1 = -1e9f;
  for (int i = 0; i < n; i++) {
    if (p[i].x < x0) x0 = p[i].x;
    if (p[i].x > x1) x1 = p[i].x;
    if (p[i].y < y0) y0 = p[i].y;
    if (p[i].y > y1) y1 = p[i].y;
  }
}

} // namespace dd
