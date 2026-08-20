// dd_skin.h — one soul, many hands.
// The doodle style (dd_face) is the canonical identity: souls that exist
// today must render identically forever. Other skins do not roll their own
// person; they receive a SoulCore extracted from the canonical traits and
// interpret it in their own language. Same seed, same soul, different artist.
#pragma once
#include "dd_core.h"
#include "dd_ink.h"
#include "dd_face.h"
#include "dd_wild.h"

namespace dd {

// The invariants every skin agrees on, derived from the canonical roll.
struct SoulCore {
  uint32_t seed = 1;
  float energy = 1;     // calm .. manic
  float warmth = 0.5f;  // cool .. hot palette bias
  int hairMass = 1;     // 0 sparse, 1 medium, 2 big
  int hairStyle = 4;    // canonical hair index (cap, buzz, ... topknot)
  int hairInk = 0;      // canonical hair ink density (solid .. light)
  int hairTint = -1;    // canonical hair colour (-1 natural, 4-5 grey, 6-9 dyed)
  int skull = 0;        // canonical skull shape (round, egg, ... bean)
  int garb = -1;        // canonical garment (-1 none, crew, hoodie, ...)
  float faceW = 0.78f;  // canonical head width ratio
  int oddEye = 0;       // the side with the special eye
  bool beardy = false;
  bool femme = false;
  int vibe = 0;         // the doodle vibe index (plain, nerd, punk...)
  int mood = 0;         // resting expression
};

static inline SoulCore soulCore(uint32_t seed) {
  FaceTraits f;
  rollFace(f, seed);            // cheap: table picks only, no drawing
  SoulCore c;
  c.seed = seed;
  int det = f.idx[C_DETAIL];
  c.energy = det == 0 ? 0.75f : det == 2 ? 1.25f : 1.0f;
  if (f.idx[C_EXPR] == 3 || f.idx[C_EXPR] == 6) c.energy += 0.1f;
  int cm = f.idx[C_COLOR];
  c.warmth = cm == 0 ? 0.35f : cm == 1 ? 0.55f : cm == 2 ? 0.7f : 0.85f;
  if (f.skinCol >= 0) c.warmth += (float)f.skinCol * 0.01f;
  int h = f.idx[C_HAIR];
  c.hairMass = (h == 8 || h == 1 || h == 6) ? 0
             : (h == 0 || h == 4 || h == 7 || h == 10 || h == 13 || h == 18) ? 1 : 2;
  c.hairStyle = h;
  c.hairInk = f.idx[C_HAIRINK];
  c.hairTint = f.hairCol;
  c.skull = f.idx[C_SKULL];
  c.faceW = f.wRatio;
  c.garb = f.garment;
  c.oddEye = f.patchSide > 0 ? 1 : 0;
  c.beardy = f.idx[C_FHAIR] >= 2;
  c.femme = f.femme;
  c.vibe = f.idx[C_VIBE];
  c.mood = f.idx[C_EXPR];
  return c;
}

// Roll wild traits as an interpretation of a canonical soul.
static inline void rollWildFromCore(WildTraits& t, const SoulCore& c) {
  rollWild(t, c.seed);
  Rng R(c.seed ^ 0x5C1AA717u);   // a separate stream for the interpretation
  t.energy = t.energy * 0.4f + c.energy * 0.6f * R.rr(0.9f, 1.15f);
  t.bigEye = c.oddEye;
  t.beardy = c.beardy;
  // the hairstyle itself carries across, translated to the wild alphabet:
  // cap/bob/emocut -> fringe, buzz/tuft -> grass, curly/messy/afro -> cloud,
  // spiky -> spikes, sidepart/long/pony -> swept, mohawk/topknot -> crest,
  // braids/buns/locs -> coils, bald/sides -> bare crown
  static const int HAIR2WILD[19] = { 2, 5, 3, 0, 1, 3, 7, 5, 7,
                                     1, 2, 6, 6, 1, 3, 4, 6, 2, 4 };
  t.hairMode = HAIR2WILD[c.hairStyle];
  // and its medium: dyed hair becomes coloured crayon, grey goes thin pen;
  // otherwise stay dark — only wispy-inked sparse hair thins to pen, and
  // scribbled hair sometimes warms to crayon
  if (c.hairTint >= 6) t.hairMedium = 1;
  else if (c.hairTint >= 4) t.hairMedium = 2;
  else if (c.hairInk == 2) t.hairMedium = R.chance(0.5f) ? 1 : 0;
  else if (c.hairInk >= 3 && c.hairMass == 0) t.hairMedium = 2;
  else t.hairMedium = 0;
  // the skull carries across too: square -> block, bean -> bulge, the rest
  // stay rounded at their canonical width (the hand may still cut facets)
  static const int SKULL2SHAPE[7] = { 0, 0, 0, 2, 0, 0, 3 };
  t.shapeMode = SKULL2SHAPE[c.skull];
  if (t.shapeMode == 0 && R.chance(0.25f)) t.shapeMode = 1;
  // and the garment: crew/tee -> tee, hoodie -> hood, studded jacket -> jacket,
  // poncho/shawl -> poncho, band-collar/blouse -> buttons, none -> mostly bare
  static const int GARB2CHEST[8] = { 0, 3, 2, 0, 5, 5, 4, 4 };
  t.chestMode = (c.garb >= 0 && c.garb <= 7) ? GARB2CHEST[c.garb]
                                             : (R.chance(0.55f) ? 6 : 0);
  t.wRatio = clampf(t.wRatio * (c.faceW / 0.78f), 0.5f, 0.95f);
  // the vibe colors the interpretation (hair now comes from the soul above)
  switch (c.vibe) {
    case 2: if (R.chance(0.75f)) t.paletteMode = R.chance(0.6f) ? 2 : 5; break; // punk: acid or pop
    case 3: if (R.chance(0.75f)) t.paletteMode = R.chance(0.5f) ? 3 : 5; break; // emo: violet or pop
    case 4: if (R.chance(0.7f)) t.paletteMode = R.chance(0.6f) ? 0 : 2; break;  // hippie
    case 5: if (R.chance(0.75f)) t.paletteMode = R.chance(0.5f) ? 5 : 4; break; // elder: muted
    case 6: if (R.chance(0.75f)) t.paletteMode = 4; break;                 // desi: earth
    case 7: if (R.chance(0.7f)) t.paletteMode = R.chance(0.5f) ? 2 : 4; break;  // rasta
    case 8: if (R.chance(0.7f)) { t.paletteMode = 4; t.energy *= 1.15f; } break; // folk
    case 1: if (t.shapeMode == 0 && R.chance(0.6f)) t.shapeMode = 2; break;     // nerd: boxy
  }
  if (c.warmth > 0.7f && t.paletteMode == 1 && R.chance(0.6f)) t.paletteMode = 0;
  if (c.warmth < 0.45f && t.paletteMode == 0 && R.chance(0.5f)) t.paletteMode = 1;
  if (c.femme && R.chance(0.4f)) t.paletteMode = R.chance(0.5f) ? 3 : t.paletteMode;
  // re-derive the mode-dependent colors after any palette change
  applyPalette(t, R);
}

// ---------------------------------------------------------------- registry
typedef void (*SkinPaperFn)(Canvas&, uint32_t seed);
typedef void (*SkinDrawFn)(Canvas&, uint8_t* cov, uint32_t seed,
                           float cx, float cy, float s, uint32_t frameSeed, int speed);

// How the host holds the soul this frame. Every field is optional: the
// zero pose with mood -1 is exactly the still portrait. Skins interpret
// what they can (wild has no eyelids to blink, no pupils to aim) and
// ignore the rest.
struct SkinPose {
  float turn = 0, pitch = 0, roll = 0, gazeX = 0;
  int mood = -1;          // -1 keeps the resting expression
  uint32_t moodSeed = 1;
  bool blink = false;
};
typedef void (*SkinPosedFn)(Canvas&, uint8_t* cov, uint32_t seed,
                            float cx, float cy, float s, uint32_t frameSeed,
                            int speed, const SkinPose& pose);

struct Skin {
  const char* name;
  SkinPaperFn paper;
  SkinDrawFn draw;     // the still portrait, by seed
  SkinPosedFn posed;   // the living one: same identity, held in a pose
};

static inline void skinDrawDoodle(Canvas& cv, uint8_t* cov, uint32_t seed,
                                  float cx, float cy, float s, uint32_t frameSeed, int speed) {
  static uint32_t cached = 0;
  static FaceTraits f;
  if (cached != seed) { rollFace(f, seed); cached = seed; }
  drawFace(cv, cov, f, cx, cy, s, frameSeed, speed);
}

static inline void skinDrawWild(Canvas& cv, uint8_t* cov, uint32_t seed,
                                float cx, float cy, float s, uint32_t frameSeed, int speed) {
  static uint32_t cached = 0;
  static WildTraits t;
  if (cached != seed) { rollWildFromCore(t, soulCore(seed)); cached = seed; }
  drawWildFace(cv, cov, t, cx, cy, s, frameSeed, speed);
}

static inline void skinPoseDoodle(Canvas& cv, uint8_t* cov, uint32_t seed,
                                  float cx, float cy, float s, uint32_t frameSeed,
                                  int speed, const SkinPose& p) {
  static uint32_t cached = 0;
  static FaceTraits base;
  if (cached != seed) { rollFace(base, seed); cached = seed; }
  FaceTraits live = base;
  if (p.mood >= 0) applyMood(live, p.mood, p.moodSeed);
  live.turn = p.turn; live.pitch = p.pitch; live.roll = p.roll; live.gazeX = p.gazeX;
  if (p.blink) live.idx[C_EYES] = 2;
  drawFace(cv, cov, live, cx, cy, s, frameSeed, speed);
}

static inline void skinPoseWild(Canvas& cv, uint8_t* cov, uint32_t seed,
                                float cx, float cy, float s, uint32_t frameSeed,
                                int speed, const SkinPose& p) {
  static uint32_t cached = 0;
  static WildTraits base;
  if (cached != seed) { rollWildFromCore(base, soulCore(seed)); cached = seed; }
  WildTraits live = base;
  if (p.mood >= 0) applyWildMood(live, p.moodSeed + (uint32_t)p.mood * 2654435761u);
  live.turn = p.turn; live.pitch = p.pitch; live.roll = p.roll;
  drawWildFace(cv, cov, live, cx, cy, s, frameSeed, speed);
}

static const Skin DD_SKINS[] = {
  { "doodle", paperBackground, skinDrawDoodle, skinPoseDoodle },
  { "wild", wildPaper, skinDrawWild, skinPoseWild },
};
static inline int skinCount() { return 2; }

} // namespace dd
