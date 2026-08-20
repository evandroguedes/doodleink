// dd_wild.h — a second, louder hand: crayon-and-marker expressionism.
// Where dd_face draws careful naive ink portraits, this one attacks the
// paper: dry pastel patches that ignore the linework, fat marker slashes,
// a thin wandering pen that draws the structure afterwards, stitch-rows
// of ticks, and stray loops of pure energy. Same seeded determinism.
#pragma once
#include "dd_core.h"
#include "dd_ink.h"

namespace dd {

// ---------------------------------------------------------------- palette
static const RGB W_PAPER = { 238, 231, 212 };
static const RGB W_PAPERS[] = {
  { 238, 231, 212 },  // cream
  { 236, 222, 216 },  // blush
  { 225, 229, 215 },  // sage
  { 221, 227, 233 },  // powder
  { 229, 226, 221 },  // warm gray
};
static const RGB W_CRAYON[] = {
  { 124, 178, 214 },  // 0 sky blue
  { 168, 206, 224 },  // 1 pale blue
  { 198, 58, 58 },    // 2 red
  { 224, 116, 126 },  // 3 pink
  { 226, 196, 86 },   // 4 yellow
  { 222, 138, 66 },   // 5 orange
  { 96, 158, 108 },   // 6 green
  { 146, 108, 178 },  // 7 purple
  { 84, 158, 158 },   // 8 teal
  { 198, 74, 142 },   // 9 magenta
  { 152, 102, 70 },   // 10 brown
  { 186, 168, 210 },  // 11 lavender
  { 172, 168, 158 },  // 12 warm gray
};
static const RGB W_MARKER[] = {
  { 46, 36, 52 },     // 0 plum black
  { 42, 48, 88 },     // 1 deep navy
  { 92, 42, 46 },     // 2 maroon
  { 38, 72, 58 },     // 3 forest
};
static const RGB W_PEN[] = {
  { 48, 62, 158 },    // 0 royal blue
  { 32, 126, 82 },    // 1 green
  { 188, 52, 46 },    // 2 red
  { 56, 44, 66 },     // 3 dark plum
};

struct WildTraits {
  uint32_t seed = 1;
  float wRatio = 0.72f;      // face half-width / half-height
  float skew = 0;            // whole-face lean
  int bigEye = 0;            // which eye is the huge dark smear
  int noseKind = 0;          // 0 open shape, 1 hooked, 2 plain double line
  int mouthBoxes = 2;
  float energy = 1;          // scales loops / ticks / diagonals
  int auraCol = 1, hotCol = 2, warmCol = 5, spareCol = 4, skyCol = 0, mouthCol = 2;
  int markerCol = 0;
  int penCol = 0, loopPen = 1, tickPen = 2;
  int paletteMode = 0;
  bool greenLoops = true, crossDiag = true, leftFence = true;
  float browY0 = -0.30f, browY1 = -0.36f, browW0 = 0.62f, browW1 = 0.34f;
  int hairMode = 0;   // 0 spike fan, 1 swept chunky slabs, 2 low fringe
  int browMode = 0;   // 0 long+short, 1 two bars, 2 one mega bar
  int haloMode = 1;   // 0 none, 1 crown halo, 2 corner loops, 3 side loop
  int shapeMode = 0;  // 0 rounded-kinked, 1 faceted, 2 block, 3 bulge
  bool beardy = false;
  int eyeGlyphBig = 0;   // 0 spiral, 1 rings, 2 crossed, 3 smear (the old one)
  int eyeGlyphSmall = 1; // 0 spiral, 1 rings, 2 ladder, 3 knot
  int noseGlyph = 0;     // 0 staircase, 1 lightning, 2 hook
  int mouthGlyph = 0;    // 0 zipper, 1 morse, 2 waves, 3 boxes (the old one)
  int browGlyph = 0;     // 0 arrow, 1 tilde run, 2 heavy slab
  int paperTint = 0;
  int annots = 3;        // how much marginalia
  int contrast = 1;      // 0 airy, 1 standard, 2 dense, 3 linework
  int hairMedium = 0;    // 0 dark marker, 1 colored crayon, 2 pen
  int chestMode = 0;     // 0 tee, 1 stripes, 2 jacket, 3 hood, 4 buttons, 5 poncho, 6 bare
  int chestCol = 0;      // which palette slot dresses the chest
  int shoulderMode = 0;  // 0 sloped, 1 squared, 2 round, 3 fallen
  float wAsym = 1;    // left/right width imbalance
  float chinShift = 0;
  float crownW = 0.75f;
  float hairAng0 = -0.5f, hairAng1 = 0.25f;
  float mouthX = 0.1f, mouthY = 0.55f;
  float noseLean = 0.06f;
  // pose, set by the host after the roll — never rolled, so every soul
  // that exists today keeps rendering identically. Same units as
  // FaceTraits: turn/pitch/roll around the resting skew.
  float turn = 0, pitch = 0, roll = 0;
};

// forward declaration: defined below rollWild
static inline void applyPalette(WildTraits& t, Rng& R);

static inline void rollWild(WildTraits& t, uint32_t seed) {
  Rng R(seed);
  t.seed = seed;
  t.wRatio = R.rr(0.52f, 0.92f);
  t.skew = R.rr(-0.08f, 0.08f);
  t.bigEye = R.chance(0.5f) ? 0 : 1;
  t.noseKind = R.ri(0, 2);
  t.mouthBoxes = R.ri(1, 3);
  t.energy = R.rr(0.7f, 1.35f);
  // palette moods: same fury, different crayon boxes
  {
    static const WOpt pm[] = { {"classic",38},{"cool",16},{"acid",14},{"violet",11},{"earth",11},{"pop",10} };
    t.paletteMode = pickW(R, pm, 6);
  }
  applyPalette(t, R);
  t.greenLoops = R.chance(0.75f);
  {
    static const WOpt hm[] = { {"spikes",11},{"swept",11},{"fringe",9},{"cloud",16},
                               {"crest",16},{"grass",14},{"coils",13},{"none",10} };
    t.hairMode = pickW(R, hm, 8);
  }
  {
    static const WOpt hmed[] = { {"dark",40},{"crayon",40},{"pen",20} };
    t.hairMedium = pickW(R, hmed, 3);
  }
  {
    static const WOpt ct[] = { {"airy",22},{"standard",40},{"dense",22},{"line",16} };
    t.contrast = pickW(R, ct, 4);
  }
  {  // the portrait is a bust: shoulders and something to wear
    static const WOpt ch[] = { {"tee",20},{"stripes",16},{"jacket",14},{"hood",12},
                               {"buttons",14},{"poncho",8},{"bare",16} };
    t.chestMode = pickW(R, ch, 7);
  }
  t.chestCol = R.ri(0, 2);
  t.browMode = R.ri(0, 2);
  t.haloMode = R.chance(0.7f) ? R.ri(1, 3) : 0;
  t.shapeMode = R.ri(0, 3);
  {  // the solid smear survives, but rarely — it was the reference's eye
    static const WOpt eg[] = { {"spiral",34},{"rings",31},{"crossed",28},{"smear",7} };
    t.eyeGlyphBig = pickW(R, eg, 4);
  }
  t.eyeGlyphSmall = R.ri(0, 3);
  t.noseGlyph = R.ri(0, 2);
  t.mouthGlyph = R.ri(0, 3);
  t.browGlyph = R.ri(0, 2);
  t.paperTint = (int)((seed * 2654435761u >> 27) % 5u);
  t.annots = R.ri(2, 4);
  t.wAsym = R.rr(0.78f, 1.22f);
  t.chinShift = R.rr(-0.28f, 0.28f);
  t.crownW = R.rr(0.55f, 0.95f);
  t.crossDiag = R.chance(0.8f);
  t.leftFence = R.chance(0.7f);
  t.browY0 = R.rr(-0.34f, -0.24f);
  t.browY1 = t.browY0 + R.rr(-0.12f, -0.02f);
  t.browW0 = R.rr(0.5f, 0.75f);
  t.browW1 = R.rr(0.25f, 0.45f);
  t.hairAng0 = R.rr(-0.7f, -0.25f);
  t.hairAng1 = R.rr(0.0f, 0.5f);
  t.mouthX = R.rr(-0.05f, 0.22f);
  t.mouthY = R.rr(0.48f, 0.62f);
  t.noseLean = R.rr(-0.04f, 0.14f);
  {  // shoulder silhouettes: sloped, coat-hanger, pillowy, or fallen away
     // (rolled last so older seeds keep every other trait)
    static const WOpt sh[] = { {"sloped",34},{"squared",26},{"round",22},{"fallen",18} };
    t.shoulderMode = pickW(R, sh, 4);
  }
}

static inline void applyPalette(WildTraits& t, Rng& R) {
  {
    #define PICK2(a, b) (R.chance(0.5f) ? (a) : (b))
    switch (t.paletteMode) {
      case 0:  // classic: the reference's box
        t.hotCol = PICK2(2, 3); t.warmCol = PICK2(5, 4); t.spareCol = R.ri(4, 6);
        t.auraCol = PICK2(1, 0); t.skyCol = 0; t.mouthCol = 2;
        t.markerCol = PICK2(0, 1); t.penCol = 0; t.loopPen = PICK2(1, 2); t.tickPen = 2;
        break;
      case 1:  // cool: blues and teals, pink heat sneaking in
        t.hotCol = PICK2(0, 8); t.warmCol = PICK2(7, 11); t.spareCol = PICK2(3, 6);
        t.auraCol = PICK2(3, 11); t.skyCol = 2; t.mouthCol = PICK2(9, 2);
        t.markerCol = PICK2(1, 0); t.penCol = PICK2(0, 3); t.loopPen = PICK2(2, 0); t.tickPen = PICK2(2, 0);
        break;
      case 2:  // acid: greens and yellows gone wrong
        t.hotCol = PICK2(6, 9); t.warmCol = 4; t.spareCol = PICK2(8, 5);
        t.auraCol = PICK2(11, 1); t.skyCol = 7; t.mouthCol = 2;
        t.markerCol = PICK2(3, 0); t.penCol = PICK2(1, 0); t.loopPen = 2; t.tickPen = PICK2(2, 1);
        break;
      case 3:  // violet hour
        t.hotCol = PICK2(7, 9); t.warmCol = PICK2(3, 11); t.spareCol = PICK2(0, 4);
        t.auraCol = 11; t.skyCol = 8; t.mouthCol = 9;
        t.markerCol = PICK2(0, 1); t.penCol = 3; t.loopPen = 1; t.tickPen = 2;
        break;
      case 4:  // earth: ochre and rust
        t.hotCol = PICK2(10, 5); t.warmCol = PICK2(4, 10); t.spareCol = PICK2(2, 6);
        t.auraCol = 1; t.skyCol = 10; t.mouthCol = 2;
        t.markerCol = PICK2(2, 0); t.penCol = PICK2(3, 0); t.loopPen = 1; t.tickPen = 2;
        break;
      default: // pop on gray: neutral face, one screaming color
        t.hotCol = PICK2(2, 9); t.warmCol = 12; t.spareCol = 12;
        t.auraCol = 12; t.skyCol = t.hotCol; t.mouthCol = t.hotCol;
        t.markerCol = 0; t.penCol = PICK2(0, 3); t.loopPen = 2; t.tickPen = 2;
        break;
    }
    #undef PICK2
  }
}

// Re-dress a wild face in a new mood: identity stays, the expression moves.
// Same contract as dd_face's applyMood — call on a COPY of the base traits.
static inline void applyWildMood(WildTraits& t, uint32_t moodSeed) {
  Rng R(moodSeed ^ t.seed * 0x9E3779B9u);
  t.browY0 = R.rr(-0.34f, -0.22f);
  t.browY1 = t.browY0 + R.rr(-0.14f, -0.02f);
  t.browW0 = R.rr(0.45f, 0.78f);
  t.browW1 = R.rr(0.22f, 0.48f);
  t.mouthY = R.rr(0.46f, 0.64f);
  t.mouthX = clampf(t.mouthX + R.rr(-0.08f, 0.08f), -0.18f, 0.36f);
  t.mouthBoxes = R.ri(1, 3);
  t.hairAng0 += R.rr(-0.12f, 0.12f);
  t.hairAng1 += R.rr(-0.12f, 0.12f);
}

// ---------------------------------------------------------------- painter
struct WildPainter {
  Ink& I;
  const WildTraits& t;
  Rng R;   // the redraw boil: reseeded every animation frame — margins,
           // color fields, stroke widths and alphas may re-improvise
  Rng G;   // the drawing itself: seeded by the soul alone, so the face's
           // structure — feature places and sizes, glyph geometry, outline
           // voices — survives the redraw instead of re-rolling per frame
  float s, w;
  int spd = 0;                           // 1 = animation frame: lighter fills

  float fillA = 1, inkA = 1, wMul = 1;   // set from t.contrast in paint()

  // pose projection, computed in paint(). The head is a shallow ellipsoid:
  // a feature's depth off the face plane swings it sideways under yaw and
  // down under pitch, so the drawing turns as one head — while margin
  // scribbles stay put on the paper where they were made.
  float pyawR = 0, pyc = 1, pys = 0, ppc = 1, pps = 0, pat = 0, pts = 1;
  float pfx0 = 0, pfy0 = 0, pfx1 = 0, pfy1 = 0;  // flow attractors, posed

  // dd_face's head, spoken rough: features sit on an ellipsoid with a
  // proper spherical depth falloff (so chin details keep to the chin and
  // the eye row leads the sweep), rotated by yaw then pitch — same order,
  // same signs, full strength. The whole recipe below mirrors dd_face:
  // features sweep on z, the bust translates with the chin as one unit,
  // and the silhouette's side rims only swell. One model, no arguments
  // between layers — that agreement is why the doodle skin reads as 3D.
  Vec2 fpos(float x, float y, float prot = 1.0f) const {
    float nx = x / w, ny = y / s;
    float zz = 1.0f - nx * nx * 0.72f - ny * ny * 0.55f;
    float z = (zz > 0 ? fastSqrt(zz) : 0) * w * 0.95f * prot;
    Vec2 v = { x * pyc + z * pys, y * ppc - z * pps };
    return v;
  }
  void poseAll(Vec2* p, int n, float prot = 1.0f) const {
    for (int i = 0; i < n; i++) p[i] = fpos(p[i].x, p[i].y, prot);
  }
  // apparent width of a feature at local x — dd_face's fshort, in spirit:
  // the side turned toward the viewer compresses as it nears the rim
  float wshort(float x) const {
    float lam = 1.1f * x / w;
    return clampf(fcos(lam + pyawR) / (fcos(lam) + 1e-3f), 0.42f, 1.12f);
  }

  WildPainter(Ink& ink, const WildTraits& tr, uint32_t strokeSeed)
    : I(ink), t(tr), R(strokeSeed ^ tr.seed ^ 0x77D1E5u),
      G(tr.seed * 0x9E3779B9u ^ 0x0FACADEu) {}

  // each feature reseeds G with its own salt, so a mood re-dress that
  // changes one feature's stroke count (extra mouth box, say) cannot
  // shift the stream and re-improvise every feature drawn after it
  void gseed(uint32_t salt) { G = Rng(t.seed * 0x9E3779B9u ^ salt); }

  // a path spoken in dots
  void dotLine(const Vec2* p, int n, RGB c, float alpha, float dotR, float gap) {
    P96 rs;
    resample(rs, p, n, gap);
    for (int i = 0; i < rs.n; i++)
      I.dot(rs.p[i].x + R.rr(-1, 1), rs.p[i].y + R.rr(-1, 1),
            dotR * R.rr(0.8f, 1.2f), c, alpha * R.rr(0.75f, 1.0f));
  }

  // two thin parallel tracks
  void railLine(const Vec2* p, int n, RGB c, float alpha, float sep) {
    P96 rs;
    resample(rs, p, n, 6.0f);
    P96 a, b;
    for (int i = 0; i < rs.n; i++) {
      Vec2 pa = rs.p[i > 0 ? i - 1 : 0], pb = rs.p[i < rs.n - 1 ? i + 1 : rs.n - 1];
      float nx = -(pb.y - pa.y), ny = pb.x - pa.x;
      float d = hypotf2(nx, ny) + 1e-4f;
      a.add(rs.p[i].x + nx / d * sep * 0.5f, rs.p[i].y + ny / d * sep * 0.5f);
      b.add(rs.p[i].x - nx / d * sep * 0.5f, rs.p[i].y - ny / d * sep * 0.5f);
    }
    I.sline(a.p, a.n, penW() * 1.8f, alpha, &c);
    I.sline(b.p, b.n, penW() * 1.8f, alpha * 0.85f, &c);
  }

  // a contour that changes instrument mid-journey: an honest pen stretch,
  // then dots, twin rails, marker, or a dry crayon drag completing the trip
  // (sometimes leaving a gap) — the retracing voices the face outline speaks.
  // N picks where the hand switches instruments: pass G so a shoulder keeps
  // its voices across frames; the widths and alphas still breathe on R.
  void mixedLine(const Vec2* p, int n, RGB pc, float alpha, float wmul = 1,
                 const RGB* dry = nullptr, Rng* N = nullptr) {
    Rng& M = N ? *N : R;
    P96 rs;
    float step = s * 0.035f > 5.0f ? s * 0.035f : 5.0f;
    resample(rs, p, n, step);
    if (rs.n < 4) { asPen(); I.sline(p, n, penW() * 2.2f * wmul, alpha, &pc); return; }
    int nseg = M.chance(0.4f) ? 1 : M.ri(2, 3);
    float cut[4];
    cut[0] = 0; cut[nseg] = 1;
    for (int k = 1; k < nseg; k++) cut[k] = (float)k / nseg + M.rr(-0.12f, 0.12f);
    static const WOpt vo[] = { {"pen",38},{"dots",15},{"rails",14},{"crayon",14},{"marker",11},{"gap",8} };
    bool drawn = false;
    for (int k = 0; k < nseg; k++) {
      int i0 = (int)(rs.n * cut[k]), i1 = (int)(rs.n * cut[k + 1]);
      if (i1 > rs.n) i1 = rs.n;
      if (i1 - i0 < 2) continue;
      int voice = pickW(M, vo, 6);
      if (voice == 5 && (nseg == 1 || (k == nseg - 1 && !drawn))) voice = 0;
      if (voice == 5) continue;   // the hand lifted; someone else finishes
      drawn = true;
      const Vec2* sp = rs.p + i0;
      int sn = i1 - i0;
      if (voice == 0) { asPen(); I.sline(sp, sn, penW() * 2.2f * wmul, alpha, &pc); }
      else if (voice == 1) dotLine(sp, sn, pc, alpha * 0.85f, penW() * 1.7f, penW() * 6.0f);
      else if (voice == 2) railLine(sp, sn, pc, alpha * 0.7f, penW() * 4.0f);
      else {
        Vec2 tri[3] = { sp[0],
                        { (sp[0].x + sp[sn - 1].x) * 0.5f + R.rr(-3, 3),
                          (sp[0].y + sp[sn - 1].y) * 0.5f + R.rr(-3, 3) },
                        sp[sn - 1] };
        if (voice == 3) { asCrayon(); gstroke(tri, 3, crayonW() * R.rr(0.5f, 0.75f), dry ? *dry : pc, alpha * 0.9f); }
        else { asMarker(); gstroke(tri, 3, markerW() * R.rr(0.55f, 0.8f), W_MARKER[t.markerCol], alpha * 0.85f); }
      }
    }
  }

  // ---- instruments ----
  void asCrayon() { I.grain = 0.8f; I.strokeFlush = 0.92f; }
  void asMarker() { I.grain = 0.16f; I.strokeFlush = 0.96f; }
  void asPen()    { I.grain = 0; I.strokeFlush = 0.85f; }

  float crayonW() { return s * R.rr(0.05f, 0.075f) * wMul; }
  float markerW() { return s * R.rr(0.045f, 0.07f) * wMul; }
  float penW()    { return s * R.rr(0.008f, 0.011f); }

  void gstroke(const Vec2* p, int n, float wd, RGB c, float alpha) {
    RGB keep = I.ink;
    I.ink = c;
    StrokeOpt o;
    o.alpha = alpha;
    o.taper = 0.05f;          // dry media has blunt ends
    o.amp = wd * 0.18f;
    I.stroke(p, n, wd, o);
    I.ink = keep;
  }

  void penline(const Vec2* p, int n, RGB c, float alpha, float wmul = 1) {
    I.sline(p, n, penW() * 2 * wmul, alpha, &c);
  }

  // a bundle of roughly parallel slashes, the basic gesture of the style.
  // N picks the geometry: R for marginalia that may boil freely, G for
  // hair and beard, whose placement belongs to the face. Widths and
  // alphas always ride R — thickness breathing is the good kind of alive.
  void slashBundle(Rng& N, float cx, float cy, float ang, float len, int n,
                   float spread, float wd, RGB c, float alpha, bool crayonTex) {
    if (crayonTex) asCrayon(); else asMarker();
    float dx = fcos(ang), dy = fsin(ang);
    float px = -dy, py = dx;
    for (int i = 0; i < n; i++) {
      float u = ((float)i / (n - 1 > 0 ? n - 1 : 1) - 0.5f) * spread;
      float jl = len * N.rr(0.75f, 1.2f);
      float bx = cx + px * u + N.rr(-len * 0.1f, len * 0.1f);
      float by = cy + py * u + N.rr(-len * 0.1f, len * 0.1f);
      float aj = ang + N.rr(-0.12f, 0.12f);
      float djx = fcos(aj), djy = fsin(aj);
      Vec2 pts[3] = { { bx - djx * jl * 0.5f, by - djy * jl * 0.5f },
                      { bx + N.rr(-2, 2), by + N.rr(-2, 2) },
                      { bx + djx * jl * 0.5f, by + djy * jl * 0.5f } };
      gstroke(pts, 3, wd * R.rr(0.8f, 1.25f), c, alpha * R.rr(0.85f, 1.0f));
    }
  }

  float fx0 = 0, fy0 = 0, fx1 = 0, fy1 = 0;  // flow attractors (the eyes)

  // one step of the flow field: tangential swirl around the nearer eye
  void flowDir(float x, float y, float ph, float& dx, float& dy) {
    float ax = x - pfx0, ay = y - pfy0;
    float bx = x - pfx1, by = y - pfy1;
    float da = ax * ax + ay * ay, db = bx * bx + by * by;
    float ux, uy, d;
    if (da < db) { d = fastSqrt(da) + 1; ux = -ay / d; uy = ax / d; }
    else { d = fastSqrt(db) + 1; ux = by / d; uy = -bx / d; }
    float n = fsin(x * 0.011f + y * 0.007f + ph) * 0.7f;
    dx = ux + n * uy + 0.15f;
    dy = uy - n * ux;
    float m = hypotf2(dx, dy) + 1e-4f;
    dx /= m; dy /= m;
  }

  // fork a placement stream: consumes exactly one roll of N no matter how
  // many strokes follow, so a speed-trimmed fill can't shift its neighbours
  uint32_t forkSeed(Rng& N) { return (uint32_t)(N.next() * 16777216.0f) * 2654435761u ^ t.seed; }

  // a field-following fill: strokes ride the current around the features.
  // N places the strokes (pass G and the fill holds its shape across
  // frames); widths and alphas keep breathing on R. The stroke count comes
  // from the nominal width so it never depends on the boil.
  void flowPatch(Rng& N, float cx, float cy, float rx, float ry, RGB c, float alpha) {
    asCrayon();
    Rng Q(forkSeed(N));
    float wd = crayonW();
    float wn = s * 0.0625f * wMul;
    float ph = Q.rr(0, 7);
    int cap = spd ? 9 : 16;
    int n = (int)(rx * ry * 4.2f / (wn * wn)) + 3;
    if (n > cap) n = cap;
    for (int i = 0; i < n; i++) {
      float a = Q.rr(0, TAU), rr_ = fastSqrt(Q.next());
      Vec2 pts[5];
      float x = cx + fcos(a) * rx * rr_, y = cy + fsin(a) * ry * rr_;
      float len = (rx + ry) * Q.rr(0.28f, 0.45f);
      pts[0] = { x, y };
      for (int k = 1; k < 5; k++) {
        float dx, dy;
        flowDir(x, y, ph, dx, dy);
        x += dx * len * 0.33f; y += dy * len * 0.33f;
        pts[k] = { x + R.rr(-2, 2), y + R.rr(-2, 2) };
      }
      gstroke(pts, 5, wd * R.rr(0.8f, 1.15f), c, alpha * R.rr(0.9f, 1.1f));
    }
  }

  // a loose gestural patch: back-and-forth crayon, deliberately unclipped.
  // N places the rows, same contract as flowPatch.
  void patch(Rng& N, float cx, float cy, float rx, float ry, float ang, RGB c, float alpha) {
    asCrayon();
    Rng Q(forkSeed(N));
    float wd = crayonW();
    float wn = s * 0.0625f * wMul;
    float dx = fcos(ang), dy = fsin(ang);
    float px = -dy, py = dx;
    int rows = (int)(2 * ry / (wn * 1.05f)) + 1;
    int cap = spd ? 8 : 12;
    if (rows > cap) rows = cap;
    for (int i = 0; i < rows; i++) {
      float u = ((float)i / (rows - 1 > 0 ? rows - 1 : 1) - 0.5f) * 2 * ry;
      float half = rx * fastSqrt(clampf(1.0f - (u / ry) * (u / ry) * 0.6f, 0.15f, 1.0f));
      float ox = cx + px * u, oy = cy + py * u;
      float over = Q.rr(-0.12f, 0.18f) * rx;
      Vec2 pts[3] = {
        { ox - dx * (half + over), oy - dy * (half + over) },
        { ox + Q.rr(-3, 3), oy + Q.rr(-3, 3) },
        { ox + dx * (half + Q.rr(-0.12f, 0.18f) * rx), oy + dy * (half + Q.rr(-0.1f, 0.2f) * rx) },
      };
      gstroke(pts, 3, wd * R.rr(0.85f, 1.1f), c, alpha * R.rr(0.95f, 1.15f));
    }
  }

  // rows of little ticks, like stitches along a seam
  void tickRow(float x0, float y0, float x1, float y1, int n, float tickLen,
               float tickAng, RGB c, float alpha) {
    asPen();
    float ca = fcos(tickAng), sa = fsin(tickAng);
    for (int i = 0; i < n; i++) {
      float ti = (float)i / (n - 1 > 0 ? n - 1 : 1);
      float bx = lerpf(x0, x1, ti) + R.rr(-2, 2);
      float by = lerpf(y0, y1, ti) + R.rr(-2, 2);
      float L = tickLen * R.rr(0.7f, 1.25f);
      Vec2 p2[2] = { { bx - ca * L * 0.5f, by - sa * L * 0.5f },
                     { bx + ca * L * 0.5f, by + sa * L * 0.5f } };
      I.sline(p2, 2, penW() * 2.6f, alpha * R.rr(0.75f, 1.0f), &c);
    }
  }

  // open pen loops, pure energy. N places them; alpha still rides R.
  void loops(Rng& N, float cx, float cy, float rx, float ry, int n, RGB c) {
    asPen();
    for (int i = 0; i < n; i++) {
      P96 a;
      float a0 = N.rr(0, TAU);
      arcPts(a, cx + N.rr(-rx * 0.3f, rx * 0.3f), cy + N.rr(-ry * 0.3f, ry * 0.3f),
             rx * N.rr(0.6f, 1.1f), ry * N.rr(0.5f, 1.0f), a0, a0 + N.rr(3.5f, 6.8f), 18);
      I.sline(a.p, a.n, penW() * 1.7f, R.rr(0.5f, 0.8f), &c);
    }
  }

  // ---- shoulders: the head gets a body to sit on ----
  // geometry rides G — a body should not re-improvise its build every
  // frame — while widths, alphas and stroke texture keep boiling on R
  void chest() {
    gseed(0xC4E57u);
    asPen();
    RGB pc = W_PEN[t.penCol];
    // dd_face's answer to the neck: the ENTIRE bust — neck, shoulders,
    // garment, collar — translates with the chin as one rigid unit
    // (its garmentAndNeck centers everything on turn*w*0.3, the same
    // carry as its chin keypoint). Nothing shears, nothing detaches.
    float nx = t.chinShift * w * 0.5f + t.turn * w * 0.30f;
    float nw2 = w * G.rr(0.2f, 0.28f);
    float ny0 = s * 0.84f, ny1 = s * G.rr(0.98f, 1.12f);
    for (int sd = -1; sd <= 1; sd += 2) {   // the neck, two honest lines
      Vec2 l[2] = { { nx + sd * nw2, ny0 }, { nx + sd * nw2 * G.rr(1.0f, 1.18f), ny1 } };
      I.sline(l, 2, penW() * 2.0f, 0.85f * inkA, &pc);
    }
    if (t.chestMode == 6) {   // bare: a stitch row where a collar would be
      tickRow(nx - w * 0.45f, ny1 + s * 0.05f, nx + w * 0.45f, ny1 + s * 0.05f,
              G.ri(3, 5), s * 0.032f, 1.57f, W_PEN[t.tickPen], 0.7f * inkA);
      return;
    }
    float sw = w * G.rr(1.35f, 2.15f);
    float sy = ny1, drop = s * G.rr(0.12f, 0.3f);
    float hem = s * G.rr(1.32f, 1.55f);
    RGB cc = W_CRAYON[t.chestCol == 0 ? t.hotCol : (t.chestCol == 1 ? t.spareCol : t.skyCol)];
    if (t.chestMode == 5) {   // poncho: one big triangle, zigzag hem
      float py2 = sy + drop + s * G.rr(0.24f, 0.34f);
      patch(G, nx, sy + s * 0.24f, sw * 0.5f, s * 0.18f, G.rr(-0.12f, 0.12f), cc, 0.55f * fillA);
      asPen();
      Vec2 l1[2] = { { nx, sy - s * 0.03f }, { nx - sw * G.rr(0.9f, 1.1f), py2 } };
      Vec2 l2[2] = { { nx, sy - s * 0.03f }, { nx + sw * G.rr(0.9f, 1.1f), py2 } };
      mixedLine(l1, 2, pc, 0.85f * inkA, 1, &cc, &G);
      mixedLine(l2, 2, pc, 0.85f * inkA, 1, &cc, &G);
      P48 hm;
      for (int i = 0; i <= 7; i++)
        hm.add(nx + lerpf(-sw * 0.96f, sw * 0.96f, (float)i / 7),
               py2 + ((i & 1) ? s * 0.05f : 0) + R.rr(-2, 2));
      I.sline(hm.p, hm.n, penW() * 2.0f, 0.75f * inkA, &pc);
      return;
    }
    // colour on the chest first, then the pen finds the edges
    patch(G, nx, sy + s * G.rr(0.22f, 0.34f), sw * G.rr(0.5f, 0.68f), s * G.rr(0.16f, 0.28f),
          G.rr(-0.12f, 0.12f), cc, 0.6f * fillA);
    asPen();
    for (int sd = -1; sd <= 1; sd += 2) {
      float swS = sw * G.rr(0.86f, 1.14f);   // shoulders come in unequal pairs
      float dropS = drop * G.rr(0.8f, 1.2f);
      P96 sm;
      if (t.shoulderMode == 1) {        // squared: a coat-hanger corner
        Vec2 sh[3] = { { nx + sd * nw2 * 1.3f, sy },
                       { nx + sd * swS * G.rr(0.82f, 0.95f), sy + dropS * G.rr(0.05f, 0.2f) },
                       { nx + sd * swS, sy + dropS } };
        sm.clear();
        for (int i = 0; i < 3; i++) sm.add(sh[i].x + R.rr(-2, 2), sh[i].y + R.rr(-2, 2));
      } else if (t.shoulderMode == 2) { // round: one pillowy curve up and over
        Vec2 sh[4] = { { nx + sd * nw2 * 1.35f, sy },
                       { nx + sd * swS * 0.55f, sy - dropS * G.rr(0.15f, 0.4f) },
                       { nx + sd * swS * G.rr(0.95f, 1.05f), sy + dropS * 0.45f },
                       { nx + sd * swS, sy + dropS } };
        chaikin(sm, sh, 4, false, 2);
      } else if (t.shoulderMode == 3) { // fallen: it all slides off steeply
        dropS *= G.rr(1.7f, 2.3f);
        swS *= G.rr(0.78f, 0.92f);
        Vec2 sh[3] = { { nx + sd * nw2 * 1.3f, sy },
                       { nx + sd * swS * 0.55f + G.rr(-4, 4), sy + dropS * 0.5f + G.rr(-4, 4) },
                       { nx + sd * swS, sy + dropS } };
        chaikin(sm, sh, 3, false, 1);
      } else {                          // sloped, the original
        Vec2 sh[3] = { { nx + sd * nw2 * 1.35f, sy },
                       { nx + sd * swS * 0.6f, sy + dropS * G.rr(0.45f, 0.6f) },
                       { nx + sd * swS, sy + dropS } };
        chaikin(sm, sh, 3, false, 1);
      }
      mixedLine(sm.p, sm.n, pc, 0.85f * inkA, 1, &cc, &G);
      if (G.chance(0.8f)) {   // side seams running off the page
        Vec2 sv[2] = { sm.p[sm.n - 1], { nx + sd * swS * G.rr(0.9f, 1.06f), hem } };
        mixedLine(sv, 2, pc, 0.7f * inkA, 0.9f, &cc, &G);
      }
    }
    if (t.chestMode == 0) {          // tee: crew neck, maybe a scribbled print
      Vec2 c2[3] = { { nx - nw2 * 1.5f, sy }, { nx + G.rr(-3, 3), sy + s * 0.07f },
                     { nx + nw2 * 1.5f, sy } };
      P96 sm2; chaikin(sm2, c2, 3, false, 1);
      mixedLine(sm2.p, sm2.n, pc, 0.85f * inkA, 1, nullptr, &G);
      if (G.chance(0.6f))
        loops(G, nx + G.rr(-0.2f, 0.2f) * w, sy + s * 0.3f, w * 0.22f, s * 0.1f,
              G.ri(1, 2), W_PEN[t.loopPen]);
    } else if (t.chestMode == 1) {   // stripes
      int nstr = G.ri(2, 4);
      for (int k = 0; k < nstr; k++)
        slashBundle(G, nx, sy + drop + s * (0.05f + 0.14f * k), G.rr(-0.06f, 0.06f),
                    sw * G.rr(1.3f, 1.6f), 1, 1, crayonW() * R.rr(0.9f, 1.3f),
                    (k & 1) ? cc : W_CRAYON[t.warmCol], 0.75f * fillA, true);
    } else if (t.chestMode == 2) {   // jacket: marker lapels + a dotted zipper
      asMarker();
      RGB dk = W_MARKER[t.markerCol];
      for (int sd = -1; sd <= 1; sd += 2) {
        Vec2 lp[2] = { { nx + sd * nw2 * 1.4f, sy },
                       { nx + sd * s * 0.05f, sy + s * G.rr(0.34f, 0.46f) } };
        gstroke(lp, 2, markerW() * R.rr(0.9f, 1.2f), dk, 0.9f * inkA);
      }
      Vec2 zp[2] = { { nx, sy + s * 0.1f }, { nx + G.rr(-4, 4), hem } };
      dotLine(zp, 2, pc, 0.7f * inkA, penW() * 1.6f, penW() * 6.5f);
    } else if (t.chestMode == 3) {   // hood: arcs at the neck + drawstrings
      asPen();
      for (int sd = -1; sd <= 1; sd += 2) {
        P96 a;
        arcPts(a, nx + sd * nw2 * 1.9f, sy - s * 0.02f, s * G.rr(0.09f, 0.12f),
               s * 0.08f, sd > 0 ? -1.4f : 1.7f, sd > 0 ? 1.7f : 4.8f, 14);
        I.sline(a.p, a.n, penW() * 2.1f, 0.8f * inkA, &pc);
        Vec2 ds[2] = { { nx + sd * nw2 * 0.7f, sy + s * 0.04f },
                       { nx + sd * nw2 * G.rr(0.5f, 0.9f), sy + s * G.rr(0.22f, 0.3f) } };
        I.sline(ds, 2, penW() * 1.8f, 0.75f * inkA, &pc);
        I.dot(ds[1].x, ds[1].y + 2, penW() * 2.6f, pc, 0.85f * inkA);
      }
    } else if (t.chestMode == 4) {   // buttons: a V and a marching placket
      Vec2 v1[2] = { { nx - nw2 * 1.4f, sy }, { nx, sy + s * G.rr(0.14f, 0.2f) } };
      Vec2 v2[2] = { { nx + nw2 * 1.4f, sy }, v1[1] };
      I.sline(v1, 2, penW() * 2.2f, 0.85f * inkA, &pc);
      I.sline(v2, 2, penW() * 2.2f, 0.85f * inkA, &pc);
      int nb = G.ri(3, 4);
      for (int k = 0; k < nb; k++)
        I.dot(nx + G.rr(-2, 2), v1[1].y + s * (0.09f + 0.12f * k),
              penW() * 2.4f, pc, 0.8f * inkA);
    }
  }

  // ---- the portrait ----
  void paint(float cx, float cy, float scale) {
    s = scale;
    w = s * t.wRatio;
    I.setXform(cx, cy, t.skew + t.roll);
    // the pose lives in the painter, not the pen: features swing on a
    // shallow ellipsoid (fpos), glyphs foreshorten (wshort), the outline
    // swells into the turn — and the marginalia stays on the paper.
    pyawR = t.turn * 0.72f;
    float pitR = t.pitch;   // full strength, dd_face's convention
    pyc = fcos(pyawR); pys = fsin(pyawR);
    ppc = fcos(pitR); pps = fsin(pitR);
    pat = fabsf(t.turn) > 1 ? 1 : fabsf(t.turn);
    pts = t.turn >= 0 ? 1 : -1;
    I.speed = 1;               // no ink crumbs; dry media has its own texture

    // contrast decides how loaded the page is
    if (t.contrast == 0)      { fillA = 0.6f;  inkA = 0.85f; wMul = 0.85f; }  // airy
    else if (t.contrast == 2) { fillA = 1.0f;  inkA = 1.0f;  wMul = 1.25f; }  // dense
    else if (t.contrast == 3) { fillA = 0.32f; inkA = 1.0f;  wMul = 0.8f;  }  // linework
    else                      { fillA = 1.0f;  inkA = 1.0f;  wMul = 1.0f;  }

    fx0 = -w * 0.34f; fy0 = -s * 0.13f;
    fx1 = w * 0.36f; fy1 = -s * 0.14f;
    Vec2 pa0 = fpos(fx0, fy0), pa1 = fpos(fx1, fy1);
    pfx0 = pa0.x; pfy0 = pa0.y; pfx1 = pa1.x; pfy1 = pa1.y;
    aura();
    gseed(0x11A70u);   // the halo hangs where it hangs; only its ink boils
    if (t.haloMode == 1)
      loops(G, G.rr(-0.4f, 0.1f) * w, -s * G.rr(0.95f, 1.15f),
            w * 0.9f, s * 0.28f, G.ri(2, 3), W_PEN[t.loopPen]);
    else if (t.haloMode == 2)
      loops(G, -w * G.rr(1.0f, 1.4f), -s * G.rr(1.1f, 1.4f),
            w * G.rr(0.8f, 1.3f), s * G.rr(0.3f, 0.5f), G.ri(2, 4), W_PEN[t.loopPen]);
    else if (t.haloMode == 3)
      loops(G, w * G.rr(0.9f, 1.2f), -s * G.rr(0.2f, 0.6f),
            w * 0.5f, s * G.rr(0.3f, 0.6f), G.ri(1, 2), W_PEN[t.loopPen]);
    chest();
    colorFields();
    hair();
    brows();
    eyes();
    nose();
    mouth();
    outline();
    ticks();
    annotations();
    if (t.crossDiag) diagonals();
    accents();
    I.grain = 0;
    I.speed = 0;
  }

  void aura() {
    gseed(0xA00A7u);   // the aura is drawn on the paper, and stays there
    RGB c = W_CRAYON[t.auraCol];
    for (int sd = -1; sd <= 1; sd += 2) {
      int n = (int)(G.rr(3, 6) * t.energy);
      slashBundle(G, sd * w * G.rr(1.15f, 1.35f), G.rr(-0.4f, 0.3f) * s,
                  1.35f + G.rr(-0.25f, 0.25f), s * G.rr(0.5f, 0.9f), n,
                  w * 0.45f, crayonW() * 0.95f, c, 0.65f * fillA, true);
    }
    // a few horizontals over the crown
    slashBundle(G, G.rr(-0.3f, 0.3f) * w, -s * G.rr(1.0f, 1.15f),
                G.rr(-0.15f, 0.15f), w * G.rr(0.7f, 1.1f), G.ri(2, 3),
                s * 0.22f, crayonW() * 0.85f, c, 0.45f * fillA, true);
  }

  void colorFields() {
    gseed(0xF1E1D5u);
    // color rides the current around the eyes; nothing lies flat. The
    // fields sit on the face, so their centers turn with the head.
    Vec2 c0 = fpos(-w * G.rr(0.15f, 0.35f), s * G.rr(0.0f, 0.2f), 0.9f);
    flowPatch(G, c0.x, c0.y,
              w * G.rr(0.5f, 0.65f), s * G.rr(0.3f, 0.42f), W_CRAYON[t.hotCol], 0.85f * fillA);
    Vec2 c1 = fpos(w * G.rr(0.2f, 0.45f), s * G.rr(-0.15f, 0.12f), 0.9f);
    flowPatch(G, c1.x, c1.y,
              w * G.rr(0.4f, 0.55f), s * G.rr(0.28f, 0.4f), W_CRAYON[t.warmCol], 0.8f * fillA);
    if (t.contrast == 3) return;   // linework keeps only those two washes
    Vec2 c2 = fpos(G.rr(-0.2f, 0.2f) * w, -s * G.rr(0.38f, 0.5f), 0.9f);
    flowPatch(G, c2.x, c2.y,
              w * G.rr(0.4f, 0.55f), s * G.rr(0.16f, 0.24f), W_CRAYON[t.hotCol], 0.7f * fillA);
    Vec2 c3 = fpos(G.rr(-0.3f, 0.3f) * w, s * G.rr(0.48f, 0.64f), 0.9f);
    flowPatch(G, c3.x, c3.y,
              w * G.rr(0.3f, 0.45f), s * G.rr(0.14f, 0.22f),
              W_CRAYON[G.chance(0.5f) ? t.hotCol : t.spareCol], 0.7f * fillA);
    int extra = (int)(G.rr(2, 4.2f) * t.energy);
    if (t.contrast == 0) extra = extra / 2;
    else if (t.contrast == 2) extra += G.ri(2, 4);
    if (spd && extra > 2) extra = 2 + (extra - 2) / 2;
    for (int i = 0; i < extra; i++) {
      Vec2 ce = fpos(G.rr(-0.55f, 0.6f) * w, G.rr(-0.3f, 0.55f) * s, 0.9f);
      flowPatch(G, ce.x, ce.y,
                w * G.rr(0.15f, 0.3f), s * G.rr(0.1f, 0.2f),
                W_CRAYON[G.chance(0.5f) ? t.spareCol : (G.chance(0.5f) ? t.warmCol : t.skyCol)], 0.75f * fillA);
    }
  }

  void hair() {
    gseed(0x4A19u);
    // the medium decides the voice: dark marker is only one option now
    bool crayonHair = (t.hairMedium == 1);
    bool penHair = (t.hairMedium == 2);
    RGB hc = crayonHair ? W_CRAYON[G.chance(0.6f) ? t.hotCol : t.spareCol]
                        : (penHair ? W_PEN[t.penCol] : W_MARKER[t.markerCol]);
    float hw = crayonHair ? crayonW() : (penHair ? penW() * 2.4f : markerW());
    float ha = (crayonHair ? 0.85f : 0.95f) * inkA;

    if (t.hairMode == 0) {         // spike fan
      Vec2 h0 = fpos(-w * G.rr(0.28f, 0.42f), -s * G.rr(0.6f, 0.75f), 1.02f);
      slashBundle(G, h0.x, h0.y,
                  1.57f + t.hairAng0, s * G.rr(0.45f, 0.6f), G.ri(6, 9),
                  w * G.rr(0.55f, 0.7f), hw * 0.9f, hc, ha, crayonHair);
      Vec2 h1 = fpos(w * G.rr(0.22f, 0.42f), -s * G.rr(0.62f, 0.78f), 1.02f);
      slashBundle(G, h1.x, h1.y,
                  1.57f + t.hairAng1, s * G.rr(0.4f, 0.55f), G.ri(6, 9),
                  w * G.rr(0.45f, 0.6f), hw * 0.9f, hc, ha, crayonHair);
    } else if (t.hairMode == 1) {  // swept slabs
      Vec2 h0 = fpos(-w * G.rr(0.2f, 0.4f), -s * G.rr(0.58f, 0.72f), 1.02f);
      slashBundle(G, h0.x, h0.y,
                  G.rr(0.45f, 0.85f), s * G.rr(0.4f, 0.55f), G.ri(4, 6),
                  w * G.rr(0.4f, 0.55f), hw * G.rr(1.2f, 1.6f), hc, ha, crayonHair);
      Vec2 h1 = fpos(w * G.rr(0.2f, 0.45f), -s * G.rr(0.62f, 0.78f), 1.02f);
      slashBundle(G, h1.x, h1.y,
                  G.rr(1.9f, 2.4f), s * G.rr(0.35f, 0.5f), G.ri(4, 6),
                  w * G.rr(0.35f, 0.5f), hw * G.rr(1.1f, 1.5f), hc, ha, crayonHair);
    } else if (t.hairMode == 2) {  // low fringe + tuft
      Vec2 h0 = fpos(G.rr(-0.15f, 0.15f) * w, -s * G.rr(0.42f, 0.52f), 1.02f);
      slashBundle(G, h0.x, h0.y,
                  G.rr(-0.15f, 0.15f), w * G.rr(0.8f, 1.1f), G.ri(3, 5),
                  s * 0.16f, hw * G.rr(1.3f, 1.7f), hc, ha, crayonHair);
      Vec2 h1 = fpos(G.rr(-0.2f, 0.3f) * w, -s * G.rr(0.75f, 0.9f), 1.02f);
      slashBundle(G, h1.x, h1.y,
                  1.57f + G.rr(-0.4f, 0.4f), s * G.rr(0.25f, 0.4f), G.ri(4, 6),
                  w * 0.35f, hw * 0.9f, hc, ha, crayonHair);
    } else if (t.hairMode == 3) {  // scribble cloud: a pen tangle over color
      Vec2 h0 = fpos(G.rr(-0.15f, 0.15f) * w, -s * G.rr(0.62f, 0.75f), 1.02f);
      flowPatch(G, h0.x, h0.y,
                w * G.rr(0.45f, 0.6f), s * G.rr(0.18f, 0.28f),
                W_CRAYON[G.chance(0.5f) ? t.spareCol : t.hotCol], 0.6f * fillA);
      asPen();
      RGB pc = penHair ? W_PEN[t.penCol] : W_PEN[t.loopPen];
      for (int k = 0; k < G.ri(3, 5); k++) {
        P96 a;
        float a0 = G.rr(0, TAU);
        Vec2 hc_ = fpos(G.rr(-0.3f, 0.3f) * w, -s * G.rr(0.58f, 0.78f), 1.02f);
        arcPts(a, hc_.x, hc_.y,
               w * G.rr(0.2f, 0.4f), s * G.rr(0.1f, 0.2f), a0, a0 + G.rr(5.0f, 8.5f), 20);
        I.sline(a.p, a.n, penW() * 2.1f, 0.75f * inkA, &pc);
      }
    } else if (t.hairMode == 4) {  // flow crest: color sweeping one way
      float ang = G.chance(0.5f) ? G.rr(-2.6f, -2.1f) : G.rr(-1.0f, -0.5f);
      for (int layer = 0; layer < 2; layer++) {
        Vec2 h0 = fpos(G.rr(-0.2f, 0.2f) * w, -s * G.rr(0.6f, 0.75f), 1.02f);
        slashBundle(G, h0.x, h0.y,
                    ang + layer * 0.15f, s * G.rr(0.4f, 0.6f), G.ri(5, 8),
                    w * G.rr(0.5f, 0.7f), crayonW() * 1.1f,
                    W_CRAYON[layer == 0 ? t.hotCol : t.spareCol], 0.8f * fillA, true);
      }
    } else if (t.hairMode == 5) {  // grass: many thin ticks along the crown
      asPen();
      RGB pc = W_PEN[t.penCol];
      int n = G.ri(14, 24);
      for (int k = 0; k < n; k++) {
        float tt = (float)k / (n - 1);
        float bx = (tt * 2 - 1) * w * G.rr(0.75f, 0.9f);
        float by = -s * (0.62f + 0.12f * fsin(tt * 3.1f)) + G.rr(-4, 4);
        float len = s * G.rr(0.07f, 0.16f);
        float aa = -1.57f + (tt * 2 - 1) * 0.5f + G.rr(-0.15f, 0.15f);
        Vec2 b0 = fpos(bx, by, 1.02f);
        Vec2 tk[2] = { b0, { b0.x + fcos(aa) * len, b0.y + fsin(aa) * len } };
        if (G.chance(0.75f)) I.sline(tk, 2, penW() * 2, 0.8f * inkA, &pc);
        else { asCrayon(); gstroke(tk, 2, crayonW() * 0.6f, W_CRAYON[t.spareCol], 0.8f * fillA); asPen(); }
      }
    } else if (t.hairMode == 6) {  // coil springs
      asPen();
      RGB pc = W_PEN[t.penCol];
      int n = G.ri(5, 8);
      for (int k = 0; k < n; k++) {
        float tt = (float)k / (n - 1);
        float bx = (tt * 2 - 1) * w * 0.7f + G.rr(-4, 4);
        float by = -s * G.rr(0.6f, 0.75f);
        float cr = s * G.rr(0.035f, 0.06f);
        Vec2 b0 = fpos(bx, by, 1.02f);
        P96 sp;
        float a0 = G.rr(0, TAU);
        for (int j = 0; j < 16; j++) {
          float jt = (float)j / 15;
          sp.add(b0.x + fcos(a0 + jt * 2.6f * TAU) * cr * (1 - 0.4f * jt),
                 b0.y + fsin(a0 + jt * 2.6f * TAU) * cr * (1 - 0.4f * jt) - jt * cr);
        }
        I.sline(sp.p, sp.n, penW() * 1.9f, 0.8f * inkA, &pc);
      }
    } else {                       // none: a nearly bare crown
      if (G.chance(0.6f)) {
        asPen();
        RGB pc = W_PEN[t.penCol];
        for (int k = 0; k < G.ri(2, 4); k++) {
          float bx = G.rr(-0.4f, 0.4f) * w, by = -s * G.rr(0.6f, 0.7f);
          Vec2 b0 = fpos(bx, by, 1.02f);
          Vec2 tk[2] = { b0, { b0.x + G.rr(-4, 4), b0.y - s * G.rr(0.04f, 0.08f) } };
          I.sline(tk, 2, penW() * 1.8f, 0.6f * inkA, &pc);
        }
      }
    }
    // thin pen loops tangled in whatever hair there is
    if (t.hairMode != 7 && G.chance(0.6f)) {
      Vec2 lc = fpos(-w * 0.5f, -s * 0.6f, 1.02f);
      loops(G, lc.x, lc.y, w * 0.26f, s * 0.16f, 1, W_PEN[t.penCol]);
    }
  }

  void brows() {
    gseed(0xB405u);
    RGB dark = W_MARKER[t.markerCol];
    float y0 = s * t.browY0, y1 = s * t.browY1;
    for (int i = 0; i < 2; i++) {
      float sd = i == 0 ? -1 : 1;
      float bx = sd * w * 0.34f, by = i == 0 ? y0 : y1;
      float bw2 = w * (i == 0 ? t.browW0 : t.browW1) * 0.55f * wshort(bx);
      if (t.browGlyph == 0) {
        // an arrow, aimed by the mood
        asPen();
        RGB pc = W_PEN[t.penCol];
        float tilt = G.rr(-0.12f, 0.2f) * s * 0.2f;
        Vec2 l[2] = { { bx - bw2, by + tilt }, { bx + bw2, by - tilt } };
        poseAll(l, 2, 1.04f);
        I.sline(l, 2, penW() * 2.6f, 0.88f, &pc);
        Vec2 h1[2] = { { bx + bw2, by - tilt }, { bx + bw2 - s * 0.045f, by - tilt - s * 0.03f } };
        Vec2 h2[2] = { { bx + bw2, by - tilt }, { bx + bw2 - s * 0.05f, by - tilt + s * 0.02f } };
        poseAll(h1, 2, 1.04f);
        poseAll(h2, 2, 1.04f);
        I.sline(h1, 2, penW() * 2.2f, 0.8f, &pc);
        I.sline(h2, 2, penW() * 2.2f, 0.8f, &pc);
      } else if (t.browGlyph == 1) {
        // a run of tildes
        asPen();
        RGB pc = W_PEN[t.penCol];
        P48 tl;
        int nw = 10;
        for (int k = 0; k <= nw; k++) {
          float tt = (float)k / nw;
          tl.add(fpos(bx - bw2 + 2 * bw2 * tt, by + fsin(tt * 12.5f) * s * 0.022f, 1.04f));
        }
        I.sline(tl.p, tl.n, penW() * 2.2f, 0.85f, &pc);
      } else {
        // the heavy slab, kept as one option among glyphs
        asMarker();
        Vec2 b0[2] = { { bx - bw2 * 1.2f, by + G.rr(-3, 3) }, { bx + bw2 * 1.2f, by + G.rr(-4, 4) } };
        poseAll(b0, 2, 1.04f);
        gstroke(b0, 2, markerW() * G.rr(1.2f, 1.6f), dark, 0.9f);
      }
    }
  }

  // every glyph takes xs: the eye's apparent width under the turn —
  // the crude 2D echo of dd_face's fshort, and what finally makes the
  // eyes read as rotating instead of sliding
  void eyeSpiral(float ex, float ey, float sz, bool heavy, float xs) {
    P96 sp;
    float turns = G.rr(2.2f, 3.2f);
    int n = 26;
    float a0 = G.rr(0, TAU);
    for (int k = 0; k < n; k++) {
      float tt = (float)k / (n - 1);
      float rr_ = sz * (1.0f - 0.82f * tt);
      sp.add(ex + fcos(a0 + tt * turns * TAU) * rr_ * xs,
             ey + fsin(a0 + tt * turns * TAU) * rr_ * 0.85f);
    }
    if (heavy) { asMarker(); gstroke(sp.p, sp.n, markerW() * 0.7f, W_MARKER[t.markerCol], 0.9f); }
    else { asPen(); RGB pc = W_PEN[t.penCol]; I.sline(sp.p, sp.n, penW() * 2.2f, 0.85f, &pc); }
  }

  void eyeRings(float ex, float ey, float sz, bool heavy, float xs) {
    asPen();
    RGB pc = heavy ? W_PEN[t.penCol] : W_PEN[t.tickPen];
    int rings = heavy ? 3 : 2;
    for (int k = 0; k < rings; k++) {
      float rr_ = sz * (1.0f - 0.3f * k) * G.rr(0.92f, 1.08f);
      P96 e;
      float a0 = G.rr(0, TAU);
      arcPts(e, ex + G.rr(-2, 2), ey + G.rr(-2, 2), rr_ * xs, rr_ * 0.85f, a0, a0 + G.rr(5.5f, 6.9f), 16);
      I.sline(e.p, e.n, penW() * 2, 0.85f, &pc);
    }
    asMarker();
    Vec2 d0[2] = { { ex - 2 * xs, ey }, { ex + 2 * xs, ey } };
    gstroke(d0, 2, markerW() * 0.6f, W_MARKER[t.markerCol], 0.9f);
  }

  void eyeCross(float ex, float ey, float sz, float xs) {
    asMarker();
    RGB dark = W_MARKER[t.markerCol];
    Vec2 a[2] = { { ex - sz * xs, ey - sz * 0.8f }, { ex + sz * xs, ey + sz * 0.8f } };
    Vec2 b[2] = { { ex - sz * xs, ey + sz * 0.8f }, { ex + sz * xs, ey - sz * 0.8f } };
    gstroke(a, 2, markerW() * 0.95f, dark, 0.92f);
    gstroke(b, 2, markerW() * 0.95f, dark, 0.92f);
  }

  void eyeLadder(float ex, float ey, float sz, float xs) {
    asPen();
    RGB pc = W_PEN[t.penCol];
    Vec2 r1[2] = { { ex - sz * xs, ey - sz * 0.7f }, { ex - sz * 0.8f * xs, ey + sz * 0.8f } };
    Vec2 r2[2] = { { ex + sz * 0.9f * xs, ey - sz * 0.7f }, { ex + sz * xs, ey + sz * 0.8f } };
    I.sline(r1, 2, penW() * 2, 0.8f, &pc);
    I.sline(r2, 2, penW() * 2, 0.8f, &pc);
    for (int k = 0; k < 3; k++) {
      float yy = ey - sz * 0.5f + k * sz * 0.5f;
      Vec2 rg[2] = { { ex - sz * 0.9f * xs, yy }, { ex + sz * 0.9f * xs, yy + G.rr(-1.5f, 1.5f) } };
      I.sline(rg, 2, penW() * 1.8f, 0.75f, &pc);
    }
  }

  void eyeSmear(float ex, float ey, float xs) {
    asMarker();
    RGB dark = W_MARKER[t.markerCol];
    for (int k = 0; k < 5; k++) {
      Vec2 p2[3] = { { ex + (-w * 0.2f + G.rr(-5, 5)) * xs, ey + G.rr(-7, 7) },
                     { ex + G.rr(-5, 5) * xs, ey + G.rr(-5, 5) },
                     { ex + (w * 0.2f + G.rr(-5, 5)) * xs, ey + G.rr(-7, 7) } };
      gstroke(p2, 3, markerW() * R.rr(1.2f, 1.7f), dark, 0.9f);
    }
  }

  void eyeKnot(float ex, float ey, float sz, float xs) {
    // a tight pen tangle, not a marker smear
    asPen();
    RGB pc = W_PEN[t.penCol];
    P96 kn;
    float a0 = G.rr(0, TAU);
    for (int j = 0; j < 26; j++) {
      float jt = (float)j / 25;
      float rr = sz * (0.4f + 0.6f * fsin(jt * 9.0f + a0));
      kn.add(ex + fcos(a0 + jt * 3.2f * TAU) * rr * xs,
             ey + fsin(a0 + jt * 3.2f * TAU) * rr * 0.8f);
    }
    I.sline(kn.p, kn.n, penW() * 2.0f, 0.85f, &pc);
  }

  void eyes() {
    gseed(0xE7E5u);
    for (int i = 0; i < 2; i++) {
      float lx = (i == 0 ? fx0 : fx1) + G.rr(-3, 3);
      float ly = (i == 0 ? fy0 : fy1) + G.rr(-3, 3);
      bool big = (i == t.bigEye);
      float sz = big ? w * G.rr(0.16f, 0.22f) : w * G.rr(0.09f, 0.13f);
      int g = big ? t.eyeGlyphBig : t.eyeGlyphSmall;
      Vec2 ec = fpos(lx, ly);        // the socket rides the head
      float xs = wshort(lx);         // and the glyph turns with it
      float ex = ec.x, ey = ec.y;
      if (big) {
        if (g == 0) eyeSpiral(ex, ey, sz, true, xs);
        else if (g == 1) eyeRings(ex, ey, sz, true, xs);
        else if (g == 2) eyeCross(ex, ey, sz, xs);
        else eyeSmear(ex, ey, xs);
      } else {
        if (g == 0) eyeSpiral(ex, ey, sz, false, xs);
        else if (g == 1) eyeRings(ex, ey, sz, false, xs);
        else if (g == 2) eyeLadder(ex, ey, sz, xs);
        else eyeKnot(ex, ey, sz, xs);
      }
    }
  }

  void nose() {
    gseed(0x905Eu);
    asPen();
    RGB pc = W_PEN[t.penCol];
    float topY = -s * 0.3f, botY = s * G.rr(0.24f, 0.34f);
    // the nose belongs to the turn: its resting lean fades as the head
    // swings left, mirrors past the middle — a page turning, never a jump.
    // The resting offset itself converges to the midline as the turn
    // grows: it is expression, not anatomy, and letting it ride made
    // right turns crowd the near rim while left turns stayed open.
    float ndir = clampf(1.0f + 2.2f * t.turn, -1.0f, 1.0f);
    float nx = (0.08f + t.noseLean) * w * ndir * (1.0f - 0.45f * pat);
    float nw2 = w * G.rr(0.14f, 0.2f);
    float xsN = wshort(nx) * ndir;   // glyph width follows, and mirrors
    if (t.noseGlyph == 0) {
      // a staircase descending the face
      P48 st;
      int steps = G.ri(3, 4);
      float x = -nw2 * 0.4f, y = topY;
      st.add(nx + x * xsN, y);
      for (int k = 0; k < steps; k++) {
        y += (botY - topY) / steps;
        st.add(nx + x * xsN, y);
        x += nw2 * (0.5f + G.rr(-0.1f, 0.2f)) * (k == steps - 1 ? -1.2f : 1.0f);
        st.add(nx + x * xsN, y);
      }
      poseAll(st.p, st.n, 1.15f);    // the nose protrudes: it leads the turn
      I.sline(st.p, st.n, penW() * 2.8f, 0.92f, &pc);
    } else if (t.noseGlyph == 1) {
      // a lightning descent, in marker
      asMarker();
      Vec2 z[4] = { { nx, topY }, { nx + nw2 * 0.8f * xsN, topY + (botY - topY) * 0.45f },
                    { nx - nw2 * 0.4f * xsN, topY + (botY - topY) * 0.6f }, { nx + nw2 * 0.5f * xsN, botY } };
      poseAll(z, 4, 1.15f);
      gstroke(z, 4, markerW() * 0.7f, W_MARKER[t.markerCol], 0.9f);
    } else {
      // the hook, kept from the old alphabet
      Vec2 p2[4] = { { nx, topY }, { nx + G.rr(-3, 3) * xsN, botY * 0.7f },
                     { nx + G.rr(-2, 4) * xsN, botY }, { nx - nw2 * xsN, botY + s * 0.02f } };
      poseAll(p2, 4, 1.15f);
      I.sline(p2, 4, penW() * 2.8f, 0.92f, &pc);
    }
    // a flow-following shade beside it (color obeys the current, not the nose)
    Vec2 sc = fpos(nx + nw2 * 1.6f * ndir, (topY + botY) * 0.45f, 0.95f);
    flowPatch(G, sc.x, sc.y, nw2 * 0.9f, (botY - topY) * 0.4f,
              W_CRAYON[t.warmCol], 0.5f);
  }

  void mouth() {
    gseed(0x300Du);
    // like the nose, the mouth's resting offset yields to the turn
    float mx = t.mouthX * w * (1.0f - 0.45f * pat), my = t.mouthY * s;
    float mw2 = w * G.rr(0.38f, 0.52f) * wshort(mx), mh = s * G.rr(0.05f, 0.08f);
    // the color smear under whatever glyph follows
    asMarker();
    Vec2 sm[3] = { { mx - mw2 * 0.7f, my + G.rr(-3, 3) }, { mx, my + G.rr(-4, 4) },
                   { mx + mw2 * 0.8f, my + G.rr(-3, 3) } };
    poseAll(sm, 3, 1.04f);
    gstroke(sm, 3, markerW() * 1.3f, W_CRAYON[t.mouthCol], 0.88f);
    asPen();
    RGB pc = W_PEN[t.penCol];
    if (t.mouthGlyph == 0) {
      // a zipper: the rail, its teeth, and end stops
      Vec2 rail[2] = { { mx - mw2, my }, { mx + mw2, my + G.rr(-3, 3) } };
      poseAll(rail, 2, 1.04f);
      I.sline(rail, 2, penW() * 2.4f, 0.9f, &pc);
      int teeth = G.ri(6, 9);
      for (int k = 0; k < teeth; k++) {
        float tt = (float)(k + 1) / (teeth + 1);
        float tx = mx - mw2 + 2 * mw2 * tt;
        float up = (k & 1) ? -1.0f : 1.0f;
        Vec2 th[2] = { { tx, my + up * 2 }, { tx + G.rr(-1, 1), my + up * mh * 0.9f } };
        poseAll(th, 2, 1.04f);
        I.sline(th, 2, penW() * 2, 0.8f, &pc);
      }
      for (int e = -1; e <= 1; e += 2) {
        float exx = mx + e * (mw2 + s * 0.02f);
        Vec2 q[4] = { { exx - 4, my - 4 }, { exx + 4, my - 4 }, { exx + 4, my + 4 }, { exx - 4, my + 4 } };
        poseAll(q, 4, 1.04f);
        I.sline(q, 4, penW() * 1.8f, 0.75f, &pc);
      }
    } else if (t.mouthGlyph == 1) {
      // morse: dashes and dots in marker
      asMarker();
      float x = mx - mw2;
      while (x < mx + mw2) {
        if (G.chance(0.55f)) {
          float len = s * G.rr(0.04f, 0.07f);
          Vec2 d0[2] = { { x, my + G.rr(-2, 2) }, { x + len, my + G.rr(-2, 2) } };
          poseAll(d0, 2, 1.04f);
          gstroke(d0, 2, markerW() * 0.8f, W_MARKER[t.markerCol], 0.9f);
          x += len + s * 0.025f;
        } else {
          Vec2 dp = fpos(x, my + G.rr(-2, 2), 1.04f);
          I.dot(dp.x, dp.y, markerW() * 0.45f, W_MARKER[t.markerCol], 0.9f);
          x += s * 0.04f;
        }
      }
    } else if (t.mouthGlyph == 2) {
      // stacked waves
      for (int r2 = 0; r2 < 2 + (t.mouthBoxes > 2 ? 1 : 0); r2++) {
        P48 wv;
        for (int k = 0; k <= 12; k++) {
          float tt = (float)k / 12;
          wv.add(fpos(mx - mw2 + 2 * mw2 * tt,
                      my + r2 * mh * 0.9f + fsin(tt * 9.5f + r2) * mh * 0.5f, 1.04f));
        }
        I.sline(wv.p, wv.n, penW() * 2.2f, 0.85f, &pc);
      }
    } else {
      // the stacked boxes, kept as one option among glyphs
      float by = my - mh * 0.5f;
      for (int b = 0; b < t.mouthBoxes; b++) {
        float ox = G.rr(-0.12f, 0.12f) * mw2;
        Vec2 q[5] = { { mx - mw2 + ox, by }, { mx + mw2 + ox, by + G.rr(-3, 3) },
                      { mx + mw2 + ox + G.rr(-4, 4), by + mh }, { mx - mw2 + ox + G.rr(-4, 4), by + mh + G.rr(-3, 3) },
                      { mx - mw2 + ox, by } };
        poseAll(q, 5, 1.04f);
        I.sline(q, 5, penW() * 2, 0.8f, &pc);
        by += mh * G.rr(0.75f, 1.0f);
      }
    }
  }

  void outline() {
    gseed(0x0DD1u);
    asPen();
    RGB pc = W_PEN[t.penCol];
    float wl = w * t.wAsym, wr = w * (2.0f - t.wAsym);
    float chin = t.chinShift * w;
    Vec2 kp[12];
    int n = 0;
    int passes = 1;
    if (t.shapeMode == 1) {
      // faceted: few points, no smoothing, a cut gem of a head
      passes = 0;
      kp[n++] = { -wl * t.crownW, -s * G.rr(0.58f, 0.68f) };
      kp[n++] = { -wl * G.rr(0.95f, 1.1f), -s * G.rr(0.05f, 0.2f) };
      kp[n++] = { -wl * G.rr(0.7f, 0.9f), s * G.rr(0.45f, 0.6f) };
      kp[n++] = { chin - w * 0.15f, s * G.rr(0.8f, 0.92f) };
      kp[n++] = { chin + w * G.rr(0.15f, 0.3f), s * G.rr(0.75f, 0.88f) };
      kp[n++] = { wr * G.rr(0.75f, 0.95f), s * G.rr(0.35f, 0.55f) };
      kp[n++] = { wr * G.rr(0.9f, 1.08f), -s * G.rr(0.1f, 0.3f) };
      kp[n++] = { wr * t.crownW, -s * G.rr(0.55f, 0.68f) };
    } else if (t.shapeMode == 2) {
      // block: flat crown, straightish sides, squared jaw
      passes = G.chance(0.5f) ? 1 : 0;
      kp[n++] = { -wl * 0.8f, -s * G.rr(0.6f, 0.7f) };
      kp[n++] = { -wl * G.rr(0.92f, 1.05f), -s * 0.35f };
      kp[n++] = { -wl * G.rr(0.9f, 1.0f), s * G.rr(0.45f, 0.6f) };
      kp[n++] = { -wl * G.rr(0.5f, 0.7f), s * G.rr(0.72f, 0.85f) };
      kp[n++] = { chin + wr * G.rr(0.4f, 0.6f), s * G.rr(0.72f, 0.85f) };
      kp[n++] = { wr * G.rr(0.85f, 1.0f), s * G.rr(0.4f, 0.55f) };
      kp[n++] = { wr * G.rr(0.9f, 1.05f), -s * 0.3f };
      kp[n++] = { wr * 0.82f, -s * G.rr(0.6f, 0.7f) };
    } else if (t.shapeMode == 3) {
      // bulge: one cheek blown out, the other collapsed
      float bl = G.chance(0.5f) ? 1.4f : 0.72f, br = 2.1f - bl;
      kp[n++] = { -wl * 0.8f, -s * 0.62f };
      kp[n++] = { -wl * bl, -s * 0.1f };
      kp[n++] = { -wl * bl * G.rr(0.8f, 0.95f), s * 0.35f };
      kp[n++] = { chin - w * 0.25f, s * G.rr(0.78f, 0.9f) };
      kp[n++] = { chin + w * G.rr(0.1f, 0.3f), s * G.rr(0.82f, 0.94f) };
      kp[n++] = { wr * br * G.rr(0.85f, 1.0f), s * 0.3f };
      kp[n++] = { wr * br, -s * 0.15f };
      kp[n++] = { wr * 0.72f, -s * G.rr(0.55f, 0.68f) };
    } else {
      // rounded with kinks, the original
      kp[n++] = { -wl * 0.85f, -s * 0.62f };
      kp[n++] = { -wl * G.rr(0.95f, 1.1f), -s * 0.2f };
      kp[n++] = { -wl * G.rr(0.9f, 1.05f), s * 0.25f };
      kp[n++] = { -wl * G.rr(0.6f, 0.8f), s * G.rr(0.6f, 0.72f) };
      kp[n++] = { chin - w * 0.2f, s * G.rr(0.78f, 0.9f) };
      kp[n++] = { chin + w * G.rr(0.15f, 0.35f), s * G.rr(0.8f, 0.92f) };
      kp[n++] = { wr * G.rr(0.65f, 0.85f), s * G.rr(0.55f, 0.7f) };
      kp[n++] = { wr * G.rr(0.95f, 1.1f), s * 0.2f };
      kp[n++] = { wr * G.rr(0.9f, 1.05f), -s * 0.3f };
      kp[n++] = { wr * 0.75f, -s * G.rr(0.55f, 0.7f) };
    }
    // rough the keypoints: corners, not curves
    for (int i = 0; i < n; i++) {
      kp[i].x += G.rr(-0.04f, 0.04f) * s;
      kp[i].y += G.rr(-0.04f, 0.04f) * s;
    }
    // the silhouette responds exactly like dd_face's: crown carries at
    // 0.16, chin leads at 0.30, the side rims only swell (the cranium
    // silhouette rotates in place), and pitch stretches crown from chin
    // (crown -0.4·pit, chin +pit) while the features sweep on the z-term
    if (t.turn != 0 || t.pitch != 0) {
      float swN = 1 + 0.10f * pat, swF = 1 - 0.26f * pat;
      float pit = t.pitch * s * 0.22f;
      for (int i = 0; i < n; i++) {
        float crown = smoothf((-kp[i].y / s - 0.35f) / 0.27f);
        float chinW2 = smoothf((kp[i].y / s - 0.55f) / 0.30f);
        kp[i].x *= (kp[i].x * pts > 0 ? swN : swF);
        kp[i].x += t.turn * w * (0.16f * crown + 0.30f * chinW2);
        kp[i].y += pit * (chinW2 - 0.4f * crown);
      }
    }
    P96 sm;
    if (passes == 0) { sm.n = 0; for (int i = 0; i < n; i++) sm.add(kp[i]); }
    else chaikin(sm, kp, n, false, passes);
    // first pass: the whole journey. later passes: stretches retraced,
    // sometimes in a different voice (dots, twin rails)
    int npass = (t.contrast == 3) ? 3 : 2;
    for (int pass = 0; pass < npass; pass++) {
      int i0 = 0, i1 = sm.n;
      if (pass > 0) { i0 = (int)(sm.n * G.rr(0.1f, 0.35f)); i1 = i0 + (int)(sm.n * G.rr(0.3f, 0.55f)); if (i1 > sm.n) i1 = sm.n; }
      P96 jit;
      jit.n = 0;
      for (int i = i0; i < i1; i++)   // the boil lives here, in the path
        jit.add(sm.p[i].x + R.rr(-4, 4) + pass * penW() * 2.5f, sm.p[i].y + R.rr(-4, 4));
      if (jit.n < 2) continue;
      if (pass == 0) { I.sline(jit.p, jit.n, penW() * 2.2f, 0.9f * inkA, &pc); continue; }
      int voice = G.ri(0, 3);
      if (voice == 1)      dotLine(jit.p, jit.n, pc, 0.7f * inkA, penW() * 1.6f, penW() * 6.0f);
      else if (voice == 2) railLine(jit.p, jit.n, pc, 0.6f * inkA, penW() * 4.0f);
      else                 I.sline(jit.p, jit.n, penW() * 2.2f, 0.6f * inkA, &pc);
    }
  }

  void annotations() {
    gseed(0xA2207u);   // margin notes live on the paper, and stay put
    asPen();
    RGB pc = W_PEN[t.tickPen];
    for (int i = 0; i < t.annots; i++) {
      int kind = G.ri(0, 2);
      if (kind == 0) {
        // an arrow pointing at something it finds important
        float tx = G.rr(-0.5f, 0.55f) * w, ty = G.rr(-0.35f, 0.6f) * s;
        float ang = G.rr(0, TAU);
        float len = s * G.rr(0.14f, 0.22f);
        Vec2 a0 = { tx + fcos(ang) * len, ty + fsin(ang) * len };
        Vec2 l[2] = { a0, { tx, ty } };
        I.sline(l, 2, penW() * 1.8f, 0.7f, &pc);
        Vec2 h1[2] = { { tx, ty }, { tx + fcos(ang + 0.5f) * len * 0.25f, ty + fsin(ang + 0.5f) * len * 0.25f } };
        Vec2 h2[2] = { { tx, ty }, { tx + fcos(ang - 0.5f) * len * 0.25f, ty + fsin(ang - 0.5f) * len * 0.25f } };
        I.sline(h1, 2, penW() * 1.6f, 0.65f, &pc);
        I.sline(h2, 2, penW() * 1.6f, 0.65f, &pc);
      } else if (kind == 1) {
        // an illegible margin note
        float bx = (G.chance(0.5f) ? -1 : 1) * w * G.rr(1.0f, 1.25f);
        float by = s * G.rr(-0.5f, 0.6f);
        P48 note;
        for (int k = 0; k <= 14; k++) {
          float tt = (float)k / 14;
          note.add(bx + tt * s * 0.16f, by + fsin(tt * 22 + R.rr(0, 3)) * s * 0.014f);
        }
        I.sline(note.p, note.n, penW() * 1.5f, 0.55f, &pc);
      } else {
        // a measurement bracket
        float bx = (G.chance(0.5f) ? -1 : 1) * w * G.rr(0.95f, 1.15f);
        float y0b = s * G.rr(-0.3f, 0.1f), y1b = y0b + s * G.rr(0.2f, 0.35f);
        Vec2 sp[2] = { { bx, y0b }, { bx, y1b } };
        I.sline(sp, 2, penW() * 1.6f, 0.6f, &pc);
        Vec2 c1[2] = { { bx, y0b }, { bx - s * 0.025f * (bx > 0 ? 1 : -1), y0b } };
        Vec2 c2[2] = { { bx, y1b }, { bx - s * 0.025f * (bx > 0 ? 1 : -1), y1b } };
        I.sline(c1, 2, penW() * 1.5f, 0.6f, &pc);
        I.sline(c2, 2, penW() * 1.5f, 0.6f, &pc);
      }
    }
  }

  void ticks() {
    gseed(0x71C05u);
    RGB rc = W_PEN[t.tickPen];
    int base = (int)(6 * t.energy);
    // under the chin, following the jaw as it turns
    Vec2 j0 = fpos(-w * 0.45f, s * 0.8f, 0.98f), j1 = fpos(w * 0.35f, s * 0.84f, 0.98f);
    tickRow(j0.x, j0.y, j1.x, j1.y, base + G.ri(2, 5),
            s * 0.035f, 1.57f, rc, 0.8f);
    // right cheek seam, riding the cheek
    Vec2 k0 = fpos(w * G.rr(0.75f, 0.95f), s * 0.25f, 0.98f);
    Vec2 k1 = fpos(w * G.rr(0.65f, 0.85f), s * 0.6f, 0.98f);
    tickRow(k0.x, k0.y, k1.x, k1.y,
            G.ri(4, 7), s * 0.03f, 0.3f, rc, 0.75f);
    // the left fence is scaffolding on the page, not on the face
    if (t.leftFence)
      tickRow(-w * G.rr(1.05f, 1.2f), -s * 0.15f, -w * G.rr(1.0f, 1.15f), s * 0.35f,
              G.ri(5, 9), s * 0.028f, 0.0f, rc, 0.75f);
  }

  void diagonals() {
    gseed(0xD1A60u);
    asPen();
    RGB pc = W_PEN[t.penCol];
    for (int i = 0; i < 2; i++) {
      float y0 = -s * G.rr(0.05f, 0.2f) + i * s * 0.07f;
      Vec2 p2[2] = { { -w * G.rr(0.75f, 0.95f), y0 },
                     { w * G.rr(0.0f, 0.3f), y0 + s * G.rr(0.28f, 0.42f) } };
      if (G.chance(0.35f)) dotLine(p2, 2, pc, 0.6f * inkA, penW() * 1.5f, penW() * 6.0f);
      else I.sline(p2, 2, penW() * 1.8f, R.rr(0.5f, 0.7f) * inkA, &pc);
    }
  }

  void accents() {
    gseed(0xACCE7u);
    // dark crayon jabs low on the face edge, turning with it
    asCrayon();
    int n = G.ri(2, 4);
    for (int i = 0; i < n; i++) {
      float ang = G.rr(0.9f, 1.8f);
      Vec2 b0 = fpos(-w * G.rr(0.55f, 0.95f), s * G.rr(0.3f, 0.75f), 0.95f);
      float L = s * G.rr(0.1f, 0.2f);
      Vec2 p2[2] = { { b0.x - fcos(ang) * L, b0.y - fsin(ang) * L },
                     { b0.x + fcos(ang) * L, b0.y + fsin(ang) * L } };
      gstroke(p2, 2, crayonW() * 0.8f, W_MARKER[0], 0.7f);
    }
    // one or two hot spots
    if (G.chance(0.7f)) {
      Vec2 hs = fpos(w * G.rr(0.5f, 0.8f), -s * G.rr(0.5f, 0.7f), 0.95f);
      patch(G, hs.x, hs.y, w * 0.1f, s * 0.05f,
            G.rr(0, 1), W_CRAYON[t.hotCol], 0.7f);
    }
    // a bearded soul keeps its beard, in slashes
    if (t.beardy) {
      gseed(0xBEA4Du);
      asMarker();
      Vec2 bc = fpos(t.chinShift * w, s * G.rr(0.72f, 0.82f), 0.98f);
      slashBundle(G, bc.x, bc.y, 1.57f + G.rr(-0.25f, 0.25f),
                  s * G.rr(0.12f, 0.2f), G.ri(4, 7), w * G.rr(0.45f, 0.65f),
                  markerW() * 0.8f, W_MARKER[t.markerCol], 0.85f, false);
    }
  }
};

// warm, toothy paper for the wild style
static inline void wildPaper(Canvas& cv, uint32_t seed) {
  int W = cv.width(), H = cv.height();
  RGB base = W_PAPERS[(seed * 2654435761u >> 27) % 5u];
  for (int y = 0; y < H; y++)
    for (int x = 0; x < W; x++) {
      uint32_t h = (uint32_t)(x * 73856093u) ^ (uint32_t)(y * 19349663u) ^ seed;
      h = (h ^ (h >> 13)) * 0x5bd1e995u;
      float n = (float)((h >> 8) & 0xFF) / 255.0f - 0.5f;
      uint8_t c[3];
      c[0] = (uint8_t)clampf(base.r + n * 10, 0, 255);
      c[1] = (uint8_t)clampf(base.g + n * 10, 0, 255);
      c[2] = (uint8_t)clampf(base.b + n * 9, 0, 255);
      cv.blend(x, y, c, 1.0f);
    }
}

static inline void drawWildFace(Canvas& cv, uint8_t* covBuf, const WildTraits& t,
                                float cx, float cy, float scale, uint32_t strokeSeed = 0,
                                int speed = 0) {
  Ink ink;
  Rng strokeR(strokeSeed ^ t.seed ^ 0x3AB1E5u);
  ink.begin(&cv, &strokeR, covBuf);
  ink.paper = W_PAPER;
  ink.grainSeed = t.seed * 2654435761u;
  WildPainter wp(ink, t, strokeSeed);
  wp.spd = speed;
  wp.paint(cx, cy, scale);
}

} // namespace dd
