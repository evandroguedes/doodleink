// dd_face.h — the casting office and the portrait painter.
// A seed rolls a character sheet (traits with rarities, like the reference
// metadata cards), a rough 3D head places the features under yaw/pitch/roll
// (Mannay's trick), and every feature is just a function — two arcs and a
// pupil make an eye, one long curve around a tip makes a nose.
#pragma once
#include "dd_core.h"
#include "dd_ink.h"

namespace dd {

static const float DEG = 0.01745329252f;

// ---------------------------------------------------------------- palettes
static const RGB PAPER_C = { 237, 233, 223 };

struct InkDef { const char* name; RGB c; uint16_t w; };
static const InkDef INKS[] = {
  { "charcoal", { 44, 42, 38 }, 58 },
  { "sepia",    { 88, 66, 46 }, 12 },
  { "oxblood",  { 116, 46, 40 }, 9 },
  { "indigo",   { 50, 58, 92 }, 11 },
  { "forest",   { 48, 70, 54 }, 6 },
  { "plum",     { 88, 54, 84 }, 4 },
};
static const int N_INKS = 6;

static const RGB SKINC[] = {
  { 246, 230, 212 }, { 242, 222, 198 }, { 230, 198, 160 }, { 212, 172, 124 },
  { 208, 162, 118 }, { 178, 126, 86 }, { 142, 98, 68 }, { 108, 76, 56 },
};
static const RGB HALOC[] = {
  { 168, 176, 160 }, { 183, 188, 205 }, { 195, 191, 208 },
  { 184, 178, 166 }, { 179, 194, 204 }, { 214, 204, 182 }, { 205, 187, 184 },
};
static const RGB ACCENTC[] = {
  { 168, 72, 60 }, { 86, 130, 120 }, { 178, 134, 58 },
  { 122, 132, 162 }, { 126, 112, 142 }, { 152, 122, 92 },
};
static const RGB BLUSH_C = { 214, 148, 140 };
static const RGB LIP_C = { 172, 74, 78 };
// hair colours: 0-3 natural warm, 4-5 grey/white (elders), 6-9 dyed (punks)
static const RGB HAIRC[] = {
  { 150, 84, 48 }, { 192, 152, 72 }, { 110, 74, 48 }, { 100, 110, 130 },
  { 172, 168, 160 }, { 214, 210, 202 }, { 186, 84, 122 }, { 74, 148, 138 },
  { 132, 92, 160 }, { 118, 128, 78 },
};

// ---------------------------------------------------------------- traits
static const WOpt T_FACING[]  = { {"straight",30},{"mild",36},{"strong",34} };
static const WOpt T_SKULL[]   = { {"round",22},{"egg",16},{"long",15},{"square",14},{"wide",13},{"small",10},{"bean",10} };
static const WOpt T_EXPR[]    = { {"neutral",28},{"happy",18},{"grumpy",14},{"surprised",9},{"sleepy",10},{"skeptic",13},{"sneer",8} };
static const WOpt T_EYES[]    = { {"round",26},{"dot",22},{"sleepy",12},{"big",14},{"oval",12},{"wink",6},{"glad",8} };
static const WOpt T_BROWS[]   = { {"none",18},{"line",30},{"thick",15},{"comma",14},{"angled",13},{"high",6},{"uni",4} };
static const WOpt T_NOSE[]    = { {"long",28},{"hook",21},{"button",14},{"triangle",10},{"big",17},{"tiny",10} };
static const WOpt T_MOUTH[]   = { {"line",26},{"smile",19},{"frown",12},{"open",12},{"o",8},{"zigzag",9},{"smirk",14} };
static const char* HAIRNAME[] = { "cap","buzz","curly","spiky","sidepart","messy","sides","tuft","bald",
                                  "long","bob","braids","buns","pony","afro","mohawk","locs","emocut","topknot" };
static const WOpt T_HAIR_M[] = { {"cap",13},{"buzz",10},{"curly",9},{"spiky",6},{"sidepart",9},{"messy",7},{"sides",6},{"tuft",3},{"bald",8},
                                 {"long",4},{"bob",2},{"braids",1},{"buns",1},{"pony",2},{"afro",5},{"mohawk",1},{"locs",2},{"emocut",1},{"topknot",3} };
static const WOpt T_HAIR_F[] = { {"cap",3},{"buzz",2},{"curly",8},{"spiky",1},{"sidepart",5},{"messy",4},{"sides",1},{"tuft",1},{"bald",1},
                                 {"long",18},{"bob",16},{"braids",10},{"buns",8},{"pony",9},{"afro",7},{"mohawk",1},{"locs",2},{"emocut",2},{"topknot",4} };
static const WOpt T_HAIRINK[] = { {"solid",30},{"hatch",22},{"scribble",20},{"stipple",14},{"light",14} };
static const WOpt T_HEADW[]   = { {"none",56},{"beanie",8},{"flatcap",6},{"headband",6},{"beret",5},
                                  {"turban",6},{"scarf",5},{"flowers",4},{"bandana",4} };
static const WOpt T_EYEW[]    = { {"none",57},{"round",12},{"square",12},{"shades",6},{"monocle",5},{"eyepatch",8} };
static const WOpt T_FHAIR[]   = { {"none",52},{"moustache",17},{"chinbeard",10},{"stubble",10},{"fullbeard",6},{"goatee",5} };
static const WOpt T_SHADE[]   = { {"none",45},{"hatch",20},{"scribble",20},{"stipple",15} };
static const WOpt T_COLOR[]   = { {"ink",34},{"skin",28},{"duo",23},{"full",15} };
static const WOpt T_DETAIL[]  = { {"sparse",25},{"normal",55},{"busy",20} };
static const WOpt T_VIBE[]    = { {"plain",33},{"nerd",9},{"punk",7},{"emo",6},{"hippie",8},{"elder",10},{"desi",8},{"rasta",5},{"folk",7} };
static const WOpt T_FEM[]     = { {"masc",53},{"femme",47} };

enum { C_FACING, C_SKULL, C_EXPR, C_EYES, C_BROWS, C_NOSE, C_MOUTH, C_HAIR,
       C_HAIRINK, C_HEADW, C_EYEW, C_FHAIR, C_SHADE, C_COLOR, C_INK, C_DETAIL, C_VIBE, C_FEM, C_COUNT };

static const char* CAT_LABELS[C_COUNT] = { "facing","skull","expression","eyes","brows","nose","mouth","hair",
  "hair ink","headwear","eyewear","facial hair","shading","colour","ink","detail","vibe","presentation" };

struct FaceTraits {
  uint32_t seed = 1;
  // categorical picks + their probabilities (for the trait card / score)
  int idx[C_COUNT];
  float pct[C_COUNT];
  // pose (re-rollable while keeping identity)
  float turn = 0, pitch = 0, roll = 0;
  // proportions
  float sizeJ = 1, wRatio = 0.68f, jaw = 0.9f, chinW = 0.8f, skullY = 0.87f, chinY = 0.8f;
  float depth = 1.15f, press = 1.1f;
  // feature params
  float eyeScale = 1, eyeSpread = 30, eyeLat = -8, eyeJitL = 1, eyeJitR = 1, eyeDyL = 0, eyeDyR = 0;
  float gazeX = 0, gazeY = 0;
  float noseLen = 1, noseHook = 1;
  float browTL = 0, browTR = 0, browRL = 0, browRR = 0;
  float mouthW = 1, mouthCurve = 0, mouthShift = 0;
  float hairLine = -30;
  int partSide = 1, patchSide = 1, brimSide = 1;
  // quirks
  bool freckles = false, blush = false, mole = false, earring = false, cheekLines = false,
       plaster = false, neckStub = false, collar = false, construction = false,
       heterochromia = false, skinScrib = false, pompom = false, teeth = false, nostril = false;
  // colors (-1 = none)
  int haloCol = -1, skinCol = -1, accentCol = -1, hairCol = -1, haloStyle = 0;
  // presentation, vibes, jewellery, garments
  bool femme = false, lashes = false, bindi = false, nosering = false, choker = false,
       beads = false, pendant = false, flowerEar = false, feather = false, facepaint = false,
       wrinkles = false, piercings = false;
  int aura = -1, garment = -1, lipCol = -1, fringeSide = 1;
  float score = 0;
};

static inline const char* traitName(const FaceTraits& f, int cat) {
  static const WOpt* tabs[] = { T_FACING, T_SKULL, T_EXPR, T_EYES, T_BROWS, T_NOSE, T_MOUTH,
                                nullptr, T_HAIRINK, T_HEADW, T_EYEW, T_FHAIR, T_SHADE, T_COLOR, nullptr, T_DETAIL,
                                T_VIBE, T_FEM };
  if (cat == C_INK) return INKS[f.idx[C_INK]].name;
  if (cat == C_HAIR) return HAIRNAME[f.idx[C_HAIR]];
  return tabs[cat][f.idx[cat]].name;
}

// Calibrated over 5000 rolls: ~62% / 22% / 9% / 4.4% / 2.7%
static inline const char* tierName(float score) {
  if (score < 36) return "common";
  if (score < 38) return "uncommon";
  if (score < 40) return "rare";
  if (score < 42) return "epic";
  return "legendary";
}

// ------------------------------------------------------------- the casting
static inline void rollPose(FaceTraits& f, uint32_t poseSeed) {
  Rng R(poseSeed);
  float p;
  f.idx[C_FACING] = pickW(R, T_FACING, 3, &p); f.pct[C_FACING] = p;
  int fi = f.idx[C_FACING];
  f.turn = fi == 0 ? R.rr(-0.12f, 0.12f)
         : fi == 1 ? R.rr(0.28f, 0.5f) * (R.chance(0.5f) ? 1 : -1)
                   : R.rr(0.58f, 0.9f) * (R.chance(0.5f) ? 1 : -1);
  f.pitch = R.rr(-0.14f, 0.18f);
  if (R.chance(0.08f)) f.pitch = R.rr(-0.3f, 0.34f);
  f.roll = R.rr(-0.09f, 0.09f);
  f.gazeX = R.rr(-1, 1) > 0 ? R.rr(0.1f, 0.5f) : R.rr(-0.5f, -0.1f);
  if (R.chance(0.6f)) f.gazeX = (f.turn >= 0 ? 1 : -1) * R.rr(0.05f, 0.45f);  // look into the turn
  f.gazeY = R.rr(-0.2f, 0.3f);
}

static inline void rollFace(FaceTraits& f, uint32_t seed) {
  Rng R(seed);
  f.seed = seed;
  float p;
  #define ROLL(cat, tab, n) { f.idx[cat] = pickW(R, tab, n, &p); f.pct[cat] = p; }

  ROLL(C_FEM, T_FEM, 2)
  f.femme = f.idx[C_FEM] == 1;
  ROLL(C_VIBE, T_VIBE, 9)
  ROLL(C_SKULL, T_SKULL, 7)
  ROLL(C_EXPR, T_EXPR, 7)
  ROLL(C_EYES, T_EYES, 7)
  ROLL(C_BROWS, T_BROWS, 7)
  ROLL(C_NOSE, T_NOSE, 6)
  ROLL(C_MOUTH, T_MOUTH, 7)
  if (f.femme) { ROLL(C_HAIR, T_HAIR_F, 19) } else { ROLL(C_HAIR, T_HAIR_M, 19) }
  ROLL(C_HAIRINK, T_HAIRINK, 5)
  ROLL(C_HEADW, T_HEADW, 9)
  ROLL(C_EYEW, T_EYEW, 6)
  ROLL(C_FHAIR, T_FHAIR, 6)
  ROLL(C_SHADE, T_SHADE, 4)
  ROLL(C_COLOR, T_COLOR, 4)
  ROLL(C_DETAIL, T_DETAIL, 3)
  {
    WOpt inkw[N_INKS];
    for (int i = 0; i < N_INKS; i++) { inkw[i].name = INKS[i].name; inkw[i].w = INKS[i].w; }
    ROLL(C_INK, inkw, N_INKS)
  }
  #undef ROLL

  // proportions
  f.sizeJ = R.rr(0.8f, 1.02f);
  f.wRatio = R.rr(0.66f, 0.9f);
  f.jaw = R.rr(0.72f, 1.05f);
  f.chinW = R.rr(0.45f, 1.1f);
  f.skullY = R.rr(0.8f, 0.94f);
  f.chinY = R.rr(0.74f, 0.88f);
  f.depth = R.rr(1.0f, 1.35f);
  f.press = R.rr(0.85f, 1.35f);
  switch (f.idx[C_SKULL]) {
    case 0: f.wRatio *= 1.07f; f.chinW = R.rr(0.8f, 1.0f); break;       // round
    case 1: f.chinW *= 0.68f; break;                                     // egg
    case 2: f.skullY *= 1.03f; f.chinY *= 1.18f; f.wRatio *= 0.9f; break;// long
    case 3: f.chinW = R.rr(1.1f, 1.35f); f.jaw *= 1.1f; break;           // square
    case 4: f.wRatio *= 1.17f; f.chinY *= 0.86f; break;                  // wide
    case 5: f.sizeJ *= 0.84f; break;                                     // small
    case 6: f.chinW *= 1.25f; f.jaw *= 1.2f; f.skullY *= 0.92f; break;   // bean (heavy jaw)
  }
  if (f.femme) {
    f.jaw *= 0.88f; f.chinW *= 0.85f; f.press *= 0.94f;
    f.idx[C_FHAIR] = 0; f.pct[C_FHAIR] = 1;
  }

  // eyes
  f.eyeScale = R.rr(0.8f, 1.18f);
  f.eyeSpread = R.rr(25, 36);
  f.eyeLat = R.rr(-14, 4);
  bool mism = R.chance(0.22f);
  f.eyeJitL = R.rr(0.85f, 1.15f) * (mism ? R.rr(0.65f, 1.5f) : 1);
  f.eyeJitR = R.rr(0.85f, 1.15f) * (mism ? R.rr(0.65f, 1.5f) : 1);
  f.eyeDyL = R.rr(-0.045f, 0.045f);
  f.eyeDyR = R.rr(-0.045f, 0.045f);

  // nose / brows / mouth flavor
  f.noseLen = R.rr(0.85f, 1.2f);
  f.noseHook = R.rr(0.7f, 1.3f);
  f.nostril = R.chance(0.4f);
  f.browTL = R.rr(-0.16f, 0.16f); f.browTR = R.rr(-0.16f, 0.16f);
  f.browRL = R.rr(-0.02f, 0.02f); f.browRR = R.rr(-0.02f, 0.02f);
  f.mouthW = R.rr(0.8f, 1.25f);
  f.mouthCurve = R.rr(-0.2f, 0.35f);
  f.mouthShift = R.rr(-0.15f, 0.15f);
  f.teeth = R.chance(0.4f);

  // expression modifiers
  switch (f.idx[C_EXPR]) {
    case 1: f.mouthCurve = R.rr(0.35f, 0.7f); break;                     // happy
    case 2: f.mouthCurve = R.rr(-0.55f, -0.25f);                          // grumpy
            f.browTL = R.rr(0.12f, 0.3f); f.browTR = -R.rr(0.12f, 0.3f); break;
    case 3: f.eyeScale *= 1.3f; break;                                    // surprised
    case 4: break;                                                        // sleepy (eye type handles)
    case 5: f.browRL = R.rr(0.04f, 0.09f); f.browTL += 0.12f; break;      // skeptic: one brow up
    case 6: f.mouthShift = (R.chance(0.5f) ? 1 : -1) * R.rr(0.2f, 0.35f); // sneer
            f.mouthCurve = R.rr(-0.35f, -0.1f); break;
  }
  if (f.idx[C_EXPR] == 3 && R.chance(0.6f)) f.idx[C_MOUTH] = 4;           // surprised: o mouth
  if (f.idx[C_EXPR] == 4) f.idx[C_EYES] = 2;                              // sleepy eyes

  f.hairLine = R.rr(-36, -24);
  if (f.idx[C_HAIR] == 0) f.hairLine = R.rr(-26, -16);                    // low cap
  f.partSide = R.chance(0.5f) ? 1 : -1;
  f.patchSide = R.chance(0.5f) ? 1 : -1;
  f.brimSide = R.chance(0.5f) ? 1 : -1;

  // quirks
  f.freckles = R.chance(0.16f);
  f.mole = R.chance(0.1f);
  f.earring = R.chance(0.09f);
  f.cheekLines = R.chance(0.28f);
  f.plaster = R.chance(0.07f);
  f.neckStub = R.chance(0.45f);
  f.collar = f.neckStub && R.chance(0.35f);
  f.construction = R.chance(0.24f);
  f.pompom = R.chance(0.3f);
  f.lashes = f.femme && R.chance(0.75f);
  if (f.femme) f.earring = R.chance(0.4f);

  // colors
  int cm = f.idx[C_COLOR];
  f.skinScrib = R.chance(0.32f);
  f.blush = false; f.heterochromia = false;
  f.haloCol = f.skinCol = f.accentCol = f.hairCol = -1;
  if (cm >= 1) f.skinCol = R.ri(0, 7);
  if (cm >= 2) f.accentCol = R.ri(0, 5);
  if (cm >= 3) { f.hairCol = R.chance(0.8f) ? R.ri(0, 3) : R.ri(6, 9); f.blush = R.chance(0.5f); }
  if (cm >= 1) f.blush = f.blush || R.chance(0.25f);
  if (cm >= 1 && R.chance(0.5f)) { f.haloCol = R.ri(0, 6); }
  f.haloStyle = R.ri(0, 4);
  if (cm >= 2) f.heterochromia = R.chance(0.08f);
  f.lipCol = (f.femme && cm >= 1 && R.chance(0.5f)) ? 0 : -1;

  // ---- the vibe: a casting profile layered over the base roll ----
  #define FORCE(cat, v) { f.idx[cat] = (v); f.pct[cat] = 1; }
  switch (f.idx[C_VIBE]) {
    case 1:  // nerd
      if (R.chance(0.85f)) FORCE(C_EYEW, R.chance(0.5f) ? 1 : 2)
      if (R.chance(0.6f)) FORCE(C_HAIR, f.femme ? (R.chance(0.5f) ? 11 : 4) : (R.chance(0.5f) ? 4 : 0))
      f.freckles = f.freckles || R.chance(0.35f);
      if (R.chance(0.5f)) f.teeth = true;
      f.garment = R.chance(0.5f) ? 6 : 0;
      f.aura = R.chance(0.4f) ? 0 : -1;
      break;
    case 2: {  // punk
      float hr = R.next();
      FORCE(C_HAIR, hr < 0.55f ? 15 : hr < 0.85f ? 3 : 1)
      FORCE(C_HEADW, 0)
      if (cm < 2) { FORCE(C_COLOR, 2) cm = 2; f.accentCol = R.ri(0, 5); if (f.skinCol < 0) f.skinCol = R.ri(0, 7); }
      f.hairCol = R.ri(6, 9);
      f.piercings = true;
      f.nosering = R.chance(0.5f);
      f.choker = R.chance(0.5f);
      if (R.chance(0.7f)) FORCE(C_EYEW, 0)
      if (f.idx[C_EXPR] == 1) FORCE(C_EXPR, R.chance(0.5f) ? 2 : 6)
      f.garment = 2;
      f.aura = R.chance(0.5f) ? 7 : -1;
      break;
    }
    case 3:  // emo
      FORCE(C_HAIR, 17)
      FORCE(C_HEADW, 0)
      f.fringeSide = f.patchSide;
      f.hairCol = R.chance(0.15f) ? 8 : -1;
      if (cm > 1) { FORCE(C_COLOR, 1) cm = 1; if (f.skinCol < 0) f.skinCol = R.ri(0, 3); f.accentCol = -1; }
      if (f.idx[C_EXPR] == 1) FORCE(C_EXPR, 0)
      if (f.idx[C_MOUTH] == 1 || f.idx[C_MOUTH] == 3) FORCE(C_MOUTH, R.chance(0.6f) ? 0 : 2)
      f.choker = R.chance(0.4f);
      f.blush = false;
      f.garment = 3;
      f.aura = R.chance(0.45f) ? 3 : -1;
      break;
    case 4: {  // hippie
      if (R.chance(0.75f)) FORCE(C_HAIR, R.chance(0.7f) ? 9 : 2)
      float hw = R.next();
      int hwPick = hw < 0.45f ? 3 : hw < 0.7f ? 7 : 0;
      FORCE(C_HEADW, hwPick)
      if (R.chance(0.45f)) FORCE(C_EYEW, 1)
      if (f.idx[C_EXPR] == 2 || f.idx[C_EXPR] == 6) FORCE(C_EXPR, 1)
      f.beads = R.chance(0.7f);
      f.pendant = R.chance(0.4f);
      f.flowerEar = R.chance(0.3f);
      if (cm < 1) { FORCE(C_COLOR, 1) cm = 1; f.skinCol = R.ri(0, 7); }
      f.garment = R.chance(0.5f) ? 4 : -1;
      f.aura = R.chance(0.6f) ? (R.chance(0.5f) ? 4 : 5) : -1;
      break;
    }
    case 5:  // elder
      f.wrinkles = true;
      f.cheekLines = true;
      if (R.chance(0.8f)) FORCE(C_HAIR, f.femme ? (R.chance(0.6f) ? 18 : 10) : (R.next() < 0.4f ? 6 : R.chance(0.5f) ? 8 : 0))
      if (R.chance(0.7f)) f.hairCol = R.chance(0.5f) ? 4 : 5;
      if (R.chance(0.55f)) FORCE(C_EYEW, R.chance(0.5f) ? 1 : 2)
      if (f.idx[C_EYES] == 3) FORCE(C_EYES, 2)
      f.garment = f.femme ? 5 : 6;
      if (f.femme) f.beads = R.chance(0.4f);
      break;
    case 6:  // desi
      if (cm < 1) { FORCE(C_COLOR, 1) cm = 1; }
      f.skinCol = R.ri(3, 7);
      if (!f.femme) {
        if (R.chance(0.55f)) { FORCE(C_HEADW, 5) if (f.accentCol < 0) f.accentCol = R.ri(0, 5); }
        if (R.chance(0.6f)) FORCE(C_FHAIR, R.chance(0.6f) ? 1 : 4)
      } else {
        f.bindi = R.chance(0.8f);
        f.nosering = R.chance(0.35f);
        if (R.chance(0.3f)) { FORCE(C_HEADW, 6) if (f.accentCol < 0) f.accentCol = R.ri(0, 5); }
        if (R.chance(0.6f)) FORCE(C_HAIR, R.chance(0.5f) ? 11 : 9)
        f.earring = R.chance(0.6f);
      }
      f.garment = 6;
      break;
    case 7:  // rasta
      FORCE(C_HAIR, 16)
      if (R.chance(0.45f)) { FORCE(C_HEADW, 1) if (f.accentCol < 0) f.accentCol = R.ri(0, 5); }
      else FORCE(C_HEADW, 0)
      if (!f.femme && R.chance(0.5f)) FORCE(C_FHAIR, R.chance(0.5f) ? 2 : 3)
      if (f.idx[C_EXPR] == 2) FORCE(C_EXPR, 1)
      f.garment = 3;
      f.aura = R.chance(0.55f) ? 6 : -1;
      break;
    case 8:  // folk
      f.facepaint = true;
      f.feather = R.chance(0.45f);
      f.beads = R.chance(0.7f);
      if (R.chance(0.7f)) FORCE(C_HAIR, R.chance(0.4f) ? 11 : R.chance(0.5f) ? 9 : 12)
      if (R.chance(0.4f)) FORCE(C_HEADW, 3)
      f.earring = f.earring || R.chance(0.5f);
      f.garment = R.chance(0.55f) ? 4 : -1;
      f.aura = R.chance(0.5f) ? 8 : -1;
      break;
    default:  // plain
      if (R.chance(0.55f)) f.garment = (f.femme && R.chance(0.3f)) ? 7 : R.ri(0, 3);
      f.aura = R.chance(0.1f) ? R.ri(0, 8) : -1;
      break;
  }
  #undef FORCE

  // score = information content of the categorical picks
  f.score = 0;
  for (int i = 0; i < C_COUNT; i++) {
    if (i == C_FACING) continue;
    f.score += -logf(f.pct[i] > 1e-6f ? f.pct[i] : 1e-6f) * 1.442695f;
  }

  rollPose(f, seed ^ 0x9E3779B9u);
}

// Re-dress an already-cast face in a new mood (for animation): keeps the
// identity, re-rolls only the expressive flavor. Work on a COPY of the
// original traits so moods don't accumulate.
static inline void applyMood(FaceTraits& f, int expr, uint32_t moodSeed) {
  Rng R(moodSeed);
  f.idx[C_EXPR] = expr;
  f.browTL = R.rr(-0.16f, 0.16f); f.browTR = R.rr(-0.16f, 0.16f);
  f.browRL = R.rr(-0.02f, 0.02f); f.browRR = R.rr(-0.02f, 0.02f);
  f.mouthCurve = R.rr(-0.2f, 0.35f);
  f.mouthShift = R.rr(-0.15f, 0.15f);
  switch (expr) {
    case 1: f.mouthCurve = R.rr(0.35f, 0.7f);
            if (R.chance(0.6f)) f.idx[C_MOUTH] = 1; break;
    case 2: f.mouthCurve = R.rr(-0.55f, -0.25f);
            f.browTL = R.rr(0.12f, 0.3f); f.browTR = -R.rr(0.12f, 0.3f);
            if (R.chance(0.5f)) f.idx[C_MOUTH] = 2; break;
    case 3: f.eyeScale *= 1.3f;
            if (R.chance(0.6f)) f.idx[C_MOUTH] = 4; break;
    case 4: f.idx[C_EYES] = 2; break;
    case 5: f.browRL = R.rr(0.04f, 0.09f); f.browTL += 0.12f; break;
    case 6: f.mouthShift = (R.chance(0.5f) ? 1 : -1) * R.rr(0.2f, 0.35f);
            f.mouthCurve = R.rr(-0.35f, -0.1f);
            if (R.chance(0.5f)) f.idx[C_MOUTH] = 6; break;
  }
}

// ------------------------------------------------------------- the painter
struct HeadGeo {
  float s, w, ay, depth, yawR, pitR;
  float turn, at, ts;
  float skullY, chinY, jaw, chinW;

  // point on (or off) the head ellipsoid, rotated and projected
  Vec2 anchor(float lonDeg, float latDeg, float prot, float* zOut = nullptr) const {
    float lon = lonDeg * DEG, lat = latDeg * DEG;
    float x = fsin(lon) * fcos(lat), y = fsin(lat), z = fcos(lon) * fcos(lat);
    x *= w * prot; y *= ay * prot; z *= w * depth * prot;
    float cy_ = fcos(yawR), sy_ = fsin(yawR);
    float x2 = x * cy_ + z * sy_;
    float z2 = -x * sy_ + z * cy_;
    float cp = fcos(pitR), sp = fsin(pitR);
    float y2 = y * cp - z2 * sp;
    float z3 = y * sp + z2 * cp;
    if (zOut) *zOut = z3 / (w * depth);
    Vec2 v = { x2, y2 };
    return v;
  }
  float fshort(float z) const { float f = 0.45f + 0.62f * clampf(z, 0, 1.1f); return clampf(f, 0.3f, 1.08f); }
  float swell(int sd) const { return 1 + (sd == (int)ts ? 0.1f : -0.28f) * at; }
};

struct FacePainter {
  Ink& I;
  const FaceTraits& f;
  HeadGeo g;
  Rng R;                 // stroke-texture rng
  float s, lwMain, lwThin;
  P96 facePoly;          // closed silhouette
  P96 outlineOpen;
  Vec2 hairline5[5];
  RGB baseInk;
  bool eyeHide[2] = { false, false };
  bool hasBehind = false;

  FacePainter(Ink& ink, const FaceTraits& ft, uint32_t strokeSeed)
    : I(ink), f(ft), R(strokeSeed ^ ft.seed) {}

  void jitter2(Vec2& v, float amt) { v.x += R.rr(-amt, amt); v.y += R.rr(-amt, amt); }

  void buildHead() {
    g.s = s;
    g.w = s * f.wRatio;
    g.ay = s * 0.9f;
    g.depth = f.depth;
    g.turn = f.turn; g.at = fabsf(f.turn); g.ts = f.turn >= 0 ? 1 : -1;
    g.yawR = f.turn * 0.72f;
    g.pitR = f.pitch;
    g.skullY = f.skullY; g.chinY = f.chinY; g.jaw = f.jaw; g.chinW = f.chinW;

    float w = g.w, at = g.at, ts = g.ts, turn = f.turn;
    float pit = f.pitch * s * 0.22f;
    Vec2 kp[12];
    int n = 0;
    // crown -> near side -> chin -> far side, near side swells with the turn
    kp[n].x = turn * w * 0.16f; kp[n].y = -s * f.skullY - pit * 0.4f; n++;   // skull top
    for (int sd = 1; sd >= -1; sd -= 2) {
      float sc = 1 + (sd == (int)ts ? 0.1f : -0.28f) * at;
      Vec2 side[5] = {
        { sd * w * 0.80f * sc, -s * f.skullY * 0.72f },
        { sd * w * 0.97f * sc, -s * 0.20f },
        { sd * w * 0.94f * sc,  s * 0.15f },
        { sd * w * 0.80f * f.jaw * sc, s * f.chinY * 0.6f },
        { sd * w * 0.34f * f.chinW * sc, s * f.chinY * 0.92f },
      };
      if (sd == 1) { for (int i = 0; i < 5; i++) { jitter2(side[i], 0.028f * s); kp[n++] = side[i]; } }
      else {
        kp[n].x = turn * w * 0.3f; kp[n].y = s * f.chinY + pit + R.rr(-0.015f, 0.015f) * s; n++;  // chin
        for (int i = 4; i >= 0; i--) { jitter2(side[i], 0.028f * s); kp[n++] = side[i]; }
      }
    }
    chaikin(facePoly, kp, n, true, 2);
    // open version for the drawn contour (start/end at the crown, overlap a bit)
    Vec2 open[14];
    for (int i = 0; i < n; i++) open[i] = kp[(i + 1) % n];
    open[n] = kp[1];
    chaikin(outlineOpen, open, n + 1, false, 2);

    for (int i = 0; i < 5; i++) {
      static const float lons[5] = { -68, -34, 0, 34, 68 };
      hairline5[i] = g.anchor(lons[i], f.hairLine, 1.02f);
      jitter2(hairline5[i], 0.02f * s);
    }
  }

  // hair/hat cap region: hairline in front, silhouette over the top
  void capPoly(P96& out, float scaleUp = 1.0f, float crownExtra = 0.0f) {
    Vec2 pts[9];
    int n = 0;
    for (int i = 0; i < 5; i++) pts[n++] = hairline5[i];
    float w = g.w, at = g.at, ts = g.ts;
    float scN = 1 + 0.1f * at, scF = 1 - 0.28f * at;
    pts[n].x = 0.80f * w * (ts > 0 ? scN : scF) * scaleUp; pts[n].y = -s * f.skullY * 0.72f * scaleUp; n++;
    pts[n].x = f.turn * w * 0.16f; pts[n].y = (-s * f.skullY - crownExtra) * scaleUp; n++;
    pts[n].x = -0.80f * w * (ts > 0 ? scF : scN) * scaleUp; pts[n].y = -s * f.skullY * 0.72f * scaleUp; n++;
    chaikin(out, pts, n, true, 2);
  }

  void toneFill(const P96& poly, int inkStyle, float sizeHint) {
    switch (inkStyle) {
      case 0: I.pencilFill(poly.p, poly.n, 1.0f, sizeHint); break;
      case 1: I.scanFill(poly.p, poly.n, I.ink, 0.06f);
              I.hatchFill(poly.p, poly.n, s * 0.055f, R.rr(1.1f, 1.7f), 0.42f, 1.15f); break;
      case 2: I.scanFill(poly.p, poly.n, I.ink, 0.05f);
              I.scribbleFill(poly.p, poly.n, s * 0.06f, 0.45f); break;
      case 3: I.scanFill(poly.p, poly.n, I.ink, 0.04f);
              I.stippleFill(poly.p, poly.n, s * 0.03f, 0.55f); break;
      default: I.scanFill(poly.p, poly.n, I.ink, 0.05f); break;           // light
    }
  }

  void closedOutline(const P96& poly, float w, float amp = -1) {
    Vec2 tmp[97];
    for (int i = 0; i < poly.n; i++) tmp[i] = poly.p[i];
    tmp[poly.n] = poly.p[0];
    StrokeOpt o; o.ghost = true; o.amp = amp;
    I.broken(tmp, poly.n + 1, w, o);
  }

  void wobEllipse(float cx, float cy, float rx, float ry, float w, float alpha) {
    P96 e;
    ellipsePts(e, cx, cy, rx, ry, 18);
    for (int i = 0; i < e.n; i++) jitter2(e.p[i], rx * 0.06f + 0.2f);
    Vec2 tmp[24];
    for (int i = 0; i < e.n; i++) tmp[i] = e.p[i];
    tmp[e.n] = e.p[0]; tmp[e.n + 1] = e.p[1];
    I.sline(tmp, e.n + 2, w, alpha);
  }

  // ------------------------------------------------ background & scaffold
  void bgDeco() {
    if (f.haloCol < 0) return;
    RGB c = HALOC[f.haloCol];
    float hrx = s * R.rr(0.78f, 0.95f), hry = s * R.rr(0.88f, 1.05f);
    float cx = R.rr(-0.06f, 0.06f) * s, cy = -s * 0.12f + R.rr(-0.05f, 0.05f) * s;
    P96 b;
    switch (f.haloStyle) {
      case 0: default:  // patch
        blobPts(b, R, cx, cy, hrx, hry, 0.12f);
        I.scanFill(b.p, b.n, c, 0.55f, 1.4f, 0.22f);
        break;
      case 1: {         // ring
        ellipsePts(b, cx, cy, hrx * 1.02f, hry * 1.02f, 26);
        for (int i = 0; i < b.n; i++) jitter2(b.p[i], s * 0.02f);
        Vec2 tmp[30]; for (int i = 0; i < b.n; i++) tmp[i] = b.p[i];
        tmp[b.n] = b.p[0]; tmp[b.n + 1] = b.p[1];
        I.sline(tmp, b.n + 2, s * 0.05f, 0.45f, &c);
        break;
      }
      case 2:           // splash
        blobPts(b, R, cx, cy, hrx, hry, 0.2f);
        { RGB keep = I.ink; I.ink = c; I.scribbleFill(b.p, b.n, s * 0.09f, 0.5f); I.ink = keep; }
        break;
      case 3: {         // dots
        int nd = R.ri(9, 15);
        for (int i = 0; i < nd; i++) {
          float a = R.rr(0, TAU), rr_ = R.rr(0.85f, 1.2f);
          I.dot(cx + fcos(a) * hrx * rr_, cy + fsin(a) * hry * rr_, s * R.rr(0.02f, 0.045f), c, R.rr(0.4f, 0.7f));
        }
        break;
      }
      case 4: {         // swoosh
        for (int k = 0; k < 2; k++) {
          P96 a;
          float a0 = R.rr(2.6f, 3.4f), a1 = a0 + R.rr(1.6f, 2.6f);
          arcPts(a, cx, cy + s * 0.05f * k, hrx * (1.02f + 0.09f * k), hry * (1.05f + 0.09f * k), a0, a1, 14);
          I.sline(a.p, a.n, s * 0.05f, 0.5f, &c);
        }
        break;
      }
    }
  }

  void scaffold() {
    if (!f.construction) return;
    P96 e;
    ellipsePts(e, 0, -s * 0.1f, g.w * 1.04f, s * 0.9f, 26);
    Vec2 tmp[30]; for (int i = 0; i < e.n; i++) tmp[i] = e.p[i];
    tmp[e.n] = e.p[0];
    I.sline(tmp, e.n + 1, 1, 0.07f);
    Vec2 v1[2] = { { f.turn * g.w * 0.2f, -s * 1.02f }, { f.turn * g.w * 0.14f, s * 0.95f } };
    I.sline(v1, 2, 1, 0.05f);
    Vec2 v2[2] = { { -g.w * 1.1f, 0 }, { g.w * 1.1f, 0 } };
    I.sline(v2, 2, 1, 0.05f);
  }

  // ------------------------------------------------ hair behind the head
  void behindMass(P96& out, float widen, float topUp, float bottomY, bool ragged) {
    float w = g.w * widen;
    Vec2 pts[16];
    int n = 0;
    pts[n].x = -w * 1.02f; pts[n].y = bottomY; n++;
    pts[n].x = -w * 1.12f; pts[n].y = s * 0.1f; n++;
    pts[n].x = -w * 1.08f; pts[n].y = -s * 0.5f; n++;
    pts[n].x = -w * 0.8f;  pts[n].y = -s * (f.skullY + topUp) * 0.92f; n++;
    pts[n].x = f.turn * g.w * 0.1f; pts[n].y = -s * (f.skullY + topUp) * 1.06f; n++;
    pts[n].x = w * 0.8f;   pts[n].y = -s * (f.skullY + topUp) * 0.92f; n++;
    pts[n].x = w * 1.08f;  pts[n].y = -s * 0.5f; n++;
    pts[n].x = w * 1.12f;  pts[n].y = s * 0.1f; n++;
    pts[n].x = w * 1.02f;  pts[n].y = bottomY; n++;
    if (ragged) {
      for (int i = 3; i >= 1 && n < 15; i--) {
        float t = (float)i / 4;
        pts[n].x = (t * 2 - 1) * w; pts[n].y = bottomY + R.rr(-0.06f, 0.1f) * s; n++;
      }
    } else { pts[n].x = 0; pts[n].y = bottomY + s * 0.03f; n++; }
    for (int i = 0; i < n; i++) jitter2(pts[i], 0.02f * s);
    chaikin(out, pts, n, true, 1);
  }

  void hairBehind() {
    int style = f.idx[C_HAIR];
    int hw = f.idx[C_HEADW];
    if (hw == 5 || hw == 6) return;                      // turban / scarf cover everything
    int inkS = f.idx[C_HAIRINK];
    RGB keep = I.ink;
    if (f.hairCol >= 0) I.ink = HAIRC[f.hairCol];
    P96 m;
    switch (style) {
      case 9:   // long
        behindMass(m, 0.95f, 0.1f, s * (f.chinY + R.rr(0.12f, 0.3f)), true);
        toneFill(m, inkS, s * 1.3f);
        closedOutline(m, lwMain * 0.8f, s * 0.03f);
        hasBehind = true;
        break;
      case 10:  // bob
        behindMass(m, 0.92f, 0.12f, s * f.chinY * R.rr(0.55f, 0.75f), false);
        toneFill(m, inkS, s * 1.1f);
        closedOutline(m, lwMain * 0.8f, s * 0.03f);
        hasBehind = true;
        break;
      case 11:  // braids: a cap of hair behind, plaits drawn in front
        behindMass(m, 0.95f, 0.1f, -s * 0.05f, false);
        toneFill(m, inkS, s * 0.8f);
        hasBehind = true;
        break;
      case 14: {  // afro
        blobPts(m, R, f.turn * g.w * 0.08f, -s * 0.32f, g.w * 1.42f, s * 0.95f, 0.11f);
        toneFill(m, inkS == 4 ? 2 : inkS, s * 1.3f);
        closedOutline(m, lwMain * 0.8f, s * 0.05f);
        for (int i = 0; i < 10; i++) {  // curls along the rim
          float a = R.rr(0, TAU);
          float ccx = f.turn * g.w * 0.08f + fcos(a) * g.w * 1.34f;
          float ccy = -s * 0.32f + fsin(a) * s * 0.9f;
          float cr = s * R.rr(0.03f, 0.05f);
          P96 c2; float a0 = R.rr(0, TAU);
          arcPts(c2, ccx, ccy, cr, cr, a0, a0 + 4.6f, 8);
          I.sline(c2.p, c2.n, lwThin, 0.6f);
        }
        hasBehind = true;
        break;
      }
      case 16:  // locs mass
        behindMass(m, 1.05f, 0.16f, s * f.chinY * 0.78f, true);
        toneFill(m, (inkS == 1 || inkS == 4) ? 2 : inkS, s);
        hasBehind = true;
        break;
      case 13: {  // ponytail swinging out one side
        float sd = -g.ts;
        float crownShift = -f.pitch * s * 0.088f;
        Vec2 base = { sd * g.w * 0.75f, -s * f.skullY * 0.6f + crownShift };
        Vec2 tip = { sd * g.w * (1.15f + R.rr(0, 0.25f)), s * R.rr(0.3f, 0.65f) };
        P96 tail;
        Vec2 tp[5] = { { base.x, base.y }, { base.x + sd * g.w * 0.42f, base.y + s * 0.2f },
                       { tip.x, tip.y }, { tip.x - sd * g.w * 0.3f, tip.y - s * 0.1f },
                       { base.x - sd * g.w * 0.05f, base.y + s * 0.16f } };
        chaikin(tail, tp, 5, true, 1);
        toneFill(tail, inkS, s * 0.6f);
        closedOutline(tail, lwThin * 1.4f, 0);
        I.dot(base.x + sd * g.w * 0.12f, base.y + s * 0.1f, lwMain * 0.8f, I.ink, 0.9f);
        hasBehind = true;
        break;
      }
      case 12: {  // two buns
        for (int sd = -1; sd <= 1; sd += 2) {
          float bcx = sd * g.w * 0.72f, bcy = -s * f.skullY * 0.88f - f.pitch * s * 0.088f;
          P96 b; blobPts(b, R, bcx, bcy, s * R.rr(0.1f, 0.13f), s * R.rr(0.09f, 0.12f), 0.14f);
          toneFill(b, inkS, s * 0.4f);
          closedOutline(b, lwThin * 1.4f, 0);
        }
        break;
      }
      case 18: {  // top knot / granny bun
        // hugging the crown, and following it as the head pitches
        float bcx = f.turn * g.w * 0.1f, bcy = -s * (f.skullY + 0.09f) - f.pitch * s * 0.088f;
        P96 b; blobPts(b, R, bcx, bcy, s * R.rr(0.13f, 0.17f), s * R.rr(0.1f, 0.13f), 0.16f);
        toneFill(b, inkS, s * 0.4f);
        closedOutline(b, lwThin * 1.4f, 0);
        Vec2 t2[2] = { { bcx - s * 0.05f, bcy + s * 0.1f }, { bcx + s * 0.05f, bcy + s * 0.1f } };
        I.sline(t2, 2, lwThin, 0.7f);
        break;
      }
    }
    I.ink = keep;
  }

  // ------------------------------------------------ skin & head contour
  void headBase() {
    if (hasBehind) I.scanFill(facePoly.p, facePoly.n, I.paper, 0.92f, 0.8f, 0.06f);
    skinWash();
  }

  void skinWash() {
    if (f.skinCol < 0) return;
    RGB c = SKINC[f.skinCol];
    // misregistered like a risograph: the wash drifts off the linework
    float dx = R.rr(-0.05f, 0.05f) * s, dy = R.rr(-0.05f, 0.05f) * s;
    float sc = R.rr(0.97f, 1.04f);
    P96 poly;
    poly.n = facePoly.n;
    for (int i = 0; i < facePoly.n; i++) {
      poly.p[i].x = facePoly.p[i].x * sc + dx;
      poly.p[i].y = facePoly.p[i].y * sc + dy;
    }
    float wa = (f.skinCol >= 6 ? 0.8f : 1.0f) / I.boost;  // washes stay light on boosted small screens
    if (f.skinScrib && s > 60) { RGB keep = I.ink; I.ink = c; I.scribbleFill(poly.p, poly.n, s * 0.075f, 0.6f * wa); I.ink = keep; }
    else I.scanFill(poly.p, poly.n, c, 0.42f * wa, 1.2f, 0.2f);
  }

  void headContour() {
    StrokeOpt o; o.over = s * 0.05f; o.ghost = true;
    o.alpha = R.rr(0.82f, 0.95f);
    I.broken(outlineOpen.p, outlineOpen.n, lwMain, o);
  }

  void shading() {
    if (f.idx[C_SHADE] == 0) return;
    // crescent along the far cheek
    float sd = -g.ts;
    P96 sh;
    Vec2 pts[8];
    int n = 0;
    for (int i = 0; i < 4; i++) {
      float lat = -18 + 24 * i;
      pts[n++] = g.anchor(sd * 62, lat, 0.99f);
    }
    for (int i = 3; i >= 0; i--) {
      float lat = -18 + 24 * i;
      pts[n++] = g.anchor(sd * 62, lat, 0.7f);
    }
    chaikin(sh, pts, n, true, 1);
    if (f.idx[C_DETAIL] == 0) return;
    switch (f.idx[C_SHADE]) {
      case 1: I.hatchFill(sh.p, sh.n, s * 0.065f, R.rr(0.9f, 1.4f), 0.1f, 1); break;
      case 2: I.scribbleFill(sh.p, sh.n, s * 0.075f, 0.09f); break;
      case 3: I.stippleFill(sh.p, sh.n, s * 0.04f, 0.14f); break;
    }
  }

  // ------------------------------------------------ ears & neck
  void ears() {
    for (int sd = -1; sd <= 1; sd += 2) {
      float z;
      Vec2 e = g.anchor(sd * 88, 4, 1.0f, &z);
      if (z < -0.15f) continue;
      e.x *= (1 + (sd == (int)g.ts ? 0.1f : -0.28f) * g.at) * 0.97f;
      float er = s * R.rr(0.06f, 0.085f);
      float a0 = sd > 0 ? -1.4f : TAU * 0.5f - 1.7f;
      P96 arc;
      arcPts(arc, e.x + sd * er * 0.35f, e.y, er, er * 1.25f, a0, a0 + 2.9f, 10);
      for (int i = 0; i < arc.n; i++) jitter2(arc.p[i], 0.3f);
      I.sline(arc.p, arc.n, lwThin * 1.4f, 0.85f);
      if (R.chance(0.5f)) {  // inner ear tick
        Vec2 t2[2] = { { e.x + sd * er * 0.1f, e.y - er * 0.2f }, { e.x + sd * er * 0.45f, e.y + er * 0.25f } };
        I.sline(t2, 2, lwThin, 0.6f);
      }
      if (f.earring && sd == (int)g.ts) {
        wobEllipse(e.x + sd * er * 0.25f, e.y + er * 1.45f, s * 0.018f, s * 0.022f, lwThin, 0.9f);
      }
    }
  }

  void neck() {
    if (!f.neckStub) return;
    float cx = f.turn * g.w * 0.3f, cy = s * f.chinY * 0.97f;
    float nw = g.w * 0.18f, len = s * R.rr(0.1f, 0.18f);
    for (int sd = -1; sd <= 1; sd += 2) {
      Vec2 l[2] = { { cx + sd * nw, cy - s * 0.04f }, { cx + sd * nw * 1.05f, cy + len } };
      I.sline(l, 2, lwThin * 1.3f, 0.8f);
    }
    if (f.collar) {
      if (R.chance(0.3f)) {  // little bow
        for (int sd = -1; sd <= 1; sd += 2) {
          Vec2 b[4] = { { cx, cy + len * 0.75f }, { cx + sd * nw * 1.1f, cy + len * 0.6f },
                        { cx + sd * nw * 1.0f, cy + len * 1.0f }, { cx, cy + len * 0.9f } };
          I.sline(b, 4, lwThin, 0.8f);
        }
      } else {
        Vec2 c2[3] = { { cx - nw * 1.5f, cy + len }, { cx, cy + len * 1.25f }, { cx + nw * 1.5f, cy + len } };
        I.sline(c2, 3, lwThin * 1.2f, 0.8f);
      }
    }
  }

  // ------------------------------------------------ hair & headwear
  void hairAndHat() {
    int hw = f.idx[C_HEADW];
    bool hatCoversHair = (hw == 1 || hw == 2 || hw == 4 || hw == 5 || hw == 6);
    RGB keep = I.ink;
    if (f.hairCol >= 0) I.ink = HAIRC[f.hairCol];
    if (!hatCoversHair) drawHairStyle();
    I.ink = keep;
    if (hw != 0) drawHeadwear();
  }

  void drawHairStyle() {
    int style = f.idx[C_HAIR];
    int inkS = f.idx[C_HAIRINK];
    P96 cap;
    switch (style) {
      case 0:  // low solid cap — the classic
        capPoly(cap);
        toneFill(cap, inkS == 4 ? 0 : inkS, s);
        hairEdge();
        break;
      case 1:  // buzz
        capPoly(cap);
        I.stippleFill(cap.p, cap.n, s * 0.028f, 0.6f);
        hairEdge(0.5f);
        break;
      case 2: {  // curly: little loops filling the cap + bumpy edge
        capPoly(cap, 1.05f, s * 0.06f);
        if (inkS == 0) toneFill(cap, 2, s);
        float x0, y0, x1, y1; bboxOf(cap.p, cap.n, x0, y0, x1, y1);
        float st = s * 0.075f;
        for (float y = y0; y < y1; y += st)
          for (float x = x0; x < x1; x += st) {
            float px = x + R.rr(-st * 0.4f, st * 0.4f), py = y + R.rr(-st * 0.4f, st * 0.4f);
            if (!pip(px, py, cap.p, cap.n)) continue;
            float cr = s * R.rr(0.025f, 0.045f);
            P96 c2;
            float a0 = R.rr(0, TAU);
            arcPts(c2, px, py, cr, cr * R.rr(0.8f, 1.2f), a0, a0 + R.rr(3.8f, 5.6f), 8);
            I.sline(c2.p, c2.n, lwThin, R.rr(0.5f, 0.85f));
          }
        break;
      }
      case 3: {  // spiky
        int ns = R.ri(10, 16);
        for (int i = 0; i < ns; i++) {
          float lon = -78 + 156 * (float)i / (ns - 1) + R.rr(-5, 5);
          Vec2 a = g.anchor(lon, f.hairLine * 0.92f, 0.985f);
          Vec2 b = g.anchor(lon + R.rr(-7, 7), f.hairLine * 1.12f, R.rr(1.06f, 1.2f));
          Vec2 sp[2] = { a, b };
          StrokeOpt o; o.wedge = true; o.alpha = R.rr(0.65f, 0.95f);
          I.stroke(sp, 2, lwMain, o);
        }
        hairEdge(0.6f);
        break;
      }
      case 4: {  // side part: sweep strokes from the part across the crown
        int nlines = R.ri(6, 9);
        float partLon = f.partSide * 38.0f;
        for (int i = 0; i < nlines; i++) {
          float t = (float)i / (nlines - 1);
          Vec2 a = g.anchor(partLon, f.hairLine + R.rr(-3, 3), 1.0f);
          Vec2 m = g.anchor(partLon * (0.4f - t * 0.9f), f.hairLine - 14 - t * 9, 1.02f + t * 0.05f);
          Vec2 b = g.anchor(-f.partSide * (30 + t * 42), f.hairLine + 4 + t * 6, 1.0f);
          Vec2 sw[3] = { a, m, b };
          I.sline(sw, 3, lwThin * R.rr(1, 1.6f), R.rr(0.5f, 0.85f));
        }
        hairEdge(0.5f);
        break;
      }
      case 5: {  // messy
        capPoly(cap, 1.03f, s * 0.04f);
        for (int i = 0; i < cap.n; i++) jitter2(cap.p[i], s * 0.035f);
        toneFill(cap, inkS == 0 ? 2 : inkS, s);
        closedOutline(cap, lwThin * 1.4f, s * 0.06f);
        int fly = R.ri(3, 6);
        for (int i = 0; i < fly; i++) {
          float lon = R.rr(-80, 80);
          Vec2 a = g.anchor(lon, f.hairLine * 1.1f, 1.0f);
          Vec2 b = g.anchor(lon + R.rr(-14, 14), f.hairLine * 1.15f, R.rr(1.08f, 1.2f));
          Vec2 sp[2] = { a, b };
          I.sline(sp, 2, lwThin, 0.6f);
        }
        break;
      }
      case 6: {  // sides only (balding)
        for (int sd = -1; sd <= 1; sd += 2) {
          float z;
          Vec2 e = g.anchor(sd * 80, 0, 1.0f, &z);
          if (z < -0.2f) continue;
          e.x *= (1 + (sd == (int)g.ts ? 0.1f : -0.28f) * g.at) * 0.96f;
          P96 b;
          blobPts(b, R, e.x, e.y, s * 0.1f, s * 0.14f, 0.14f);
          toneFill(b, inkS == 4 ? 3 : inkS, s * 0.4f);
          closedOutline(b, lwThin, 0);
        }
        // a few lonely strands on top
        for (int i = 0; i < 3; i++) {
          float lon = R.rr(-16, 16);
          Vec2 a = g.anchor(lon, -55, 1.0f);
          Vec2 b = g.anchor(lon + R.rr(-6, 6), -58, R.rr(1.1f, 1.22f));
          Vec2 sp[2] = { a, b };
          I.sline(sp, 2, lwThin, 0.55f);
        }
        break;
      }
      case 7: {  // tuft
        Vec2 c = g.anchor(f.turn * 12, -62, 1.02f);
        P96 b;
        blobPts(b, R, c.x, c.y - s * 0.05f, s * 0.12f, s * 0.1f, 0.22f);
        toneFill(b, inkS, s * 0.4f);
        closedOutline(b, lwThin * 1.3f, 0);
        break;
      }
      case 9: {  // long: centre part + curtains framing the face
        Vec2 part = g.anchor(f.turn * 8, f.hairLine - 4, 1.0f);
        for (int sd = -1; sd <= 1; sd += 2) {
          float sw = g.swell(sd);
          Vec2 c1[4] = { part,
                         g.anchor(sd * 46, f.hairLine + 2, 1.03f),
                         { sd * g.w * 0.95f * sw, -s * 0.1f },
                         { sd * g.w * 0.92f * sw, s * f.chinY * 0.55f } };
          P96 sm; chaikin(sm, c1, 4, false, 2);
          StrokeOpt o; o.alpha = 0.85f;
          I.stroke(sm.p, sm.n, lwMain * 0.9f, o);
          if (R.chance(0.7f)) {
            Vec2 st[3] = { { part.x + sd * s * 0.03f, part.y + s * 0.03f },
                           g.anchor(sd * 40, f.hairLine + 14, 1.0f),
                           { sd * g.w * 0.8f * sw, s * 0.05f } };
            I.sline(st, 3, lwThin, 0.55f);
          }
        }
        break;
      }
      case 10: {  // bob: straight bangs + curtains to the jaw
        capPoly(cap, 1.0f, s * 0.02f);
        toneFill(cap, inkS == 4 ? 0 : inkS, s);
        hairEdge();
        for (int sd = -1; sd <= 1; sd += 2) {
          float sw = g.swell(sd);
          Vec2 c1[3] = { { sd * g.w * 0.88f * sw, -s * 0.35f },
                         { sd * g.w * 1.0f * sw, s * 0.05f },
                         { sd * g.w * 0.9f * sw, s * f.chinY * 0.62f } };
          StrokeOpt o; o.alpha = 0.88f;
          I.stroke(c1, 3, lwMain, o);
        }
        break;
      }
      case 11: {  // braids: parted cap + two plaits of little loops
        capPoly(cap, 1.0f, s * 0.03f);
        toneFill(cap, inkS == 4 ? 2 : inkS, s);
        hairEdge();
        Vec2 pl[2] = { g.anchor(0, f.hairLine, 1.0f), g.anchor(f.turn * 6, -62, 1.04f) };
        RGB pc = I.paper;
        I.sline(pl, 2, lwThin, 0.7f, &pc);
        for (int sd = -1; sd <= 1; sd += 2) {
          float z;
          Vec2 e = g.anchor(sd * 82, 6, 1.02f, &z);
          if (z < -0.2f) continue;
          e.x *= g.swell(sd);
          int nb = R.ri(3, 5);
          float bx = e.x, by = e.y + s * 0.06f;
          float br = s * R.rr(0.045f, 0.06f);
          for (int k = 0; k < nb; k++) {
            float off = (k & 1) ? br * 0.5f : -br * 0.5f;
            wobEllipse(bx + sd * off * 0.7f, by, br * 0.85f, br * 0.7f, lwThin * 1.3f, 0.85f);
            by += br * 1.15f;
            bx += sd * s * 0.012f;
          }
          Vec2 t2[2] = { { bx - br * 0.5f, by - br * 0.3f }, { bx + br * 0.5f, by - br * 0.4f } };
          I.sline(t2, 2, lwThin, 0.8f);
          Vec2 tuft[2] = { { bx, by - br * 0.2f }, { bx + sd * s * 0.02f, by + br * 0.6f } };
          I.sline(tuft, 2, lwThin, 0.6f);
        }
        break;
      }
      case 12: case 18: {  // buns / top knot: slicked hair over the skull
        capPoly(cap);
        if (inkS != 4) toneFill(cap, inkS, s);
        hairEdge(0.7f);
        int nl = 3;
        for (int i = 0; i < nl; i++) {
          float t = (float)i / (nl - 1);
          Vec2 a = g.anchor((t * 2 - 1) * 48, f.hairLine + 2, 1.0f);
          Vec2 b;
          if (style == 18) { b.x = f.turn * g.w * 0.1f; b.y = -s * f.skullY * 0.92f; }
          else { b.x = ((t < 0.5f) ? -1 : 1) * g.w * 0.6f; b.y = -s * f.skullY * 0.8f; }
          Vec2 l[2] = { a, b };
          I.sline(l, 2, lwThin, 0.32f);
        }
        break;
      }
      case 13: {  // ponytail front: slicked back
        capPoly(cap);
        if (inkS != 4) toneFill(cap, inkS, s);
        hairEdge(0.8f);
        float sd = -g.ts;
        for (int i = 0; i < 4; i++) {
          Vec2 a = g.anchor(-sd * 40 + i * sd * 26.0f, f.hairLine + R.rr(-2, 4), 1.0f);
          Vec2 b = { sd * g.w * 0.72f, -s * f.skullY * 0.62f };
          Vec2 l[2] = { a, b };
          I.sline(l, 2, lwThin, 0.5f);
        }
        break;
      }
      case 14:  // afro front: the mass behind does the work
        hairEdge(0.85f);
        break;
      case 15: {  // mohawk: bare sides, a fan of tall spikes
        for (int sd = -1; sd <= 1; sd += 2) {
          float z;
          Vec2 e = g.anchor(sd * 70, -22, 1.0f, &z);
          if (z < -0.1f) continue;
          for (int k = 0; k < 6; k++)
            I.dot(e.x + R.rr(-1, 1) * s * 0.07f, e.y + R.rr(-1, 1) * s * 0.06f, s * 0.008f + 0.3f, I.ink, R.rr(0.3f, 0.5f));
        }
        {  // dark base ridge along the crown
          Vec2 rb[3] = { g.anchor(f.turn * 10 - 26, -50, 0.97f), g.anchor(f.turn * 10, -56, 0.99f),
                         g.anchor(f.turn * 10 + 26, -50, 0.97f) };
          StrokeOpt ob; ob.alpha = 0.9f;
          I.stroke(rb, 3, lwMain * 1.6f, ob);
        }
        int ns2 = R.ri(6, 8);
        for (int i = 0; i < ns2; i++) {
          float t = (float)i / (ns2 - 1);
          float lon = f.turn * 10 + (t * 2 - 1) * 26;
          Vec2 a = g.anchor(lon, -52, 0.95f);
          float fan = (t * 2 - 1) * 0.55f + f.turn * 0.25f;
          Vec2 b = { a.x + fan * s * 0.28f, a.y - s * R.rr(0.42f, 0.58f) };
          Vec2 sp[2] = { a, b };
          StrokeOpt o; o.wedge = true; o.alpha = R.rr(0.85f, 0.98f); o.amp = 0.5f;
          I.stroke(sp, 2, lwMain * 1.9f, o);
        }
        break;
      }
      case 16: {  // locs: a solid crown, thick ropes hanging beside the face
        capPoly(cap, 1.02f, s * 0.05f);
        toneFill(cap, (inkS == 1 || inkS == 4) ? 2 : inkS, s);
        hairEdge(0.85f);
        for (int sd = -1; sd <= 1; sd += 2) {
          int nl = R.ri(3, 5);
          for (int i = 0; i < nl; i++) {
            float t = (float)i / (nl - 1 > 0 ? nl - 1 : 1);
            float lon = sd * (40 + t * 46);
            float z;
            Vec2 a = g.anchor(lon, f.hairLine + 6, 1.04f, &z);
            if (z < -0.35f) continue;
            a.x *= g.swell(sd);
            float hang = s * R.rr(0.5f, 0.95f) * (0.55f + 0.45f * t);
            float sway = sd * s * R.rr(0.04f, 0.1f);
            Vec2 l[3] = { a, { a.x + sway + R.rr(-2, 2), a.y + hang * 0.5f },
                          { a.x + sway * R.rr(0.6f, 1.4f), a.y + hang } };
            P96 sm; chaikin(sm, l, 3, false, 1);
            StrokeOpt o; o.alpha = R.rr(0.8f, 0.95f); o.taper = 0.08f;
            I.stroke(sm.p, sm.n, lwMain * R.rr(1.25f, 1.6f), o);
            I.dot(l[2].x, l[2].y, lwMain * 0.7f, I.ink, 0.9f);
          }
        }
        break;
      }
      case 17: {  // emo fringe: a swoop covering the near eye
        float sd = g.ts;
        eyeHide[sd > 0 ? 1 : 0] = true;
        Vec2 fp2[6];
        fp2[0] = g.anchor(-sd * 55, f.hairLine - 2, 1.02f);
        fp2[1] = g.anchor(-sd * 10, f.hairLine + 6, 1.02f);
        fp2[2] = g.anchor(sd * 34, 6, 1.02f);
        fp2[3] = g.anchor(sd * 72, -6, 1.04f);
        fp2[4] = g.anchor(sd * 55, -50, 1.02f);
        fp2[5] = g.anchor(-sd * 25, -62, 1.03f);
        P96 fr;
        chaikin(fr, fp2, 6, true, 1);
        toneFill(fr, (inkS == 1 || inkS == 4) ? 0 : inkS, s);
        closedOutline(fr, lwThin * 1.4f, 0);
        for (int k = 0; k < 3; k++) {  // spiky fringe tips
          Vec2 a = g.anchor(sd * (30 + k * 16.0f), 2 + k * 3.0f, 1.02f);
          Vec2 b = { a.x + sd * s * 0.03f, a.y + s * R.rr(0.05f, 0.09f) };
          Vec2 sp[2] = { a, b };
          StrokeOpt o; o.wedge = true; o.alpha = 0.85f;
          I.stroke(sp, 2, lwMain * 0.8f, o);
        }
        break;
      }
      default: {  // bald: one shine arc
        P96 a;
        Vec2 c = g.anchor(-g.ts * 22, -48, 1.0f);
        arcPts(a, c.x, c.y, s * 0.16f, s * 0.1f, 3.4f, 4.9f, 8);
        I.sline(a.p, a.n, lwThin, 0.35f);
        break;
      }
    }
  }

  void hairEdge(float alpha = 0.9f) {
    // draw the hairline front edge
    P96 e;
    chaikin(e, hairline5, 5, false, 2);
    I.sline(e.p, e.n, lwThin * 1.7f, alpha);
  }

  void drawHeadwear() {
    int hw = f.idx[C_HEADW];
    RGB accent = f.accentCol >= 0 ? ACCENTC[f.accentCol] : I.ink;
    bool colored = f.accentCol >= 0;
    P96 cap;
    switch (hw) {
      case 1: {  // beanie (a slouchy striped tam for rastas)
        bool tam = f.idx[C_VIBE] == 7;
        capPoly(cap, tam ? 1.14f : 1.07f, s * (tam ? 0.18f : 0.1f));
        I.scanFill(cap.p, cap.n, I.paper, 0.8f, 1.0f, 0.05f);
        if (colored) { I.scanFill(cap.p, cap.n, accent, 0.75f, 1.2f, 0.18f); }
        else toneFill(cap, f.idx[C_HAIRINK] == 4 ? 1 : f.idx[C_HAIRINK], s);
        closedOutline(cap, lwMain * 0.85f);
        // ribbed band along the brim
        P96 band;
        chaikin(band, hairline5, 5, false, 2);
        I.sline(band.p, band.n, lwMain, 0.9f);
        if (tam) {  // brim stripes
          static const int stripeCols[2] = { 0, 2 };  // brick red, mustard
          for (int k2 = 0; k2 < 2; k2++) {
            RGB sc = ACCENTC[stripeCols[k2]];
            P48 st;
            for (int i = 0; i < band.n; i += 2)
              st.add(band.p[i].x, band.p[i].y - s * (0.045f + 0.045f * k2));
            I.sline(st.p, st.n, lwThin * 1.6f, 0.8f, &sc);
          }
        }
        for (int i = 1; i < 8; i++) {
          float t = (float)i / 8.0f;
          int bi = (int)(t * (band.n - 1));
          Vec2 v[2] = { { band.p[bi].x, band.p[bi].y - s * 0.055f }, { band.p[bi].x + R.rr(-1, 1), band.p[bi].y + s * 0.01f } };
          I.sline(v, 2, lwThin, 0.55f);
        }
        if (f.pompom) {
          Vec2 c = g.anchor(f.turn * 10, -66, 1.14f);
          P96 b; blobPts(b, R, c.x, c.y, s * 0.05f, s * 0.05f, 0.25f);
          I.stippleFill(b.p, b.n, s * 0.02f, 0.7f);
          closedOutline(b, lwThin, 0);
        }
        break;
      }
      case 2: {  // flat cap
        capPoly(cap, 1.09f, s * 0.02f);
        I.scanFill(cap.p, cap.n, I.paper, 0.8f, 1.0f, 0.05f);
        if (colored) I.scanFill(cap.p, cap.n, accent, 0.7f, 1.2f, 0.18f);
        else toneFill(cap, f.idx[C_HAIRINK] == 4 ? 1 : f.idx[C_HAIRINK], s);
        closedOutline(cap, lwMain * 0.8f);
        // brim juts where the face points
        float bl = f.brimSide >= 0 ? g.ts : -g.ts;
        Vec2 b0 = g.anchor(bl * 26, f.hairLine + 4, 1.0f);
        Vec2 b1 = g.anchor(bl * 68, f.hairLine + 9, 1.3f);
        Vec2 br[3] = { b0, { (b0.x + b1.x) * 0.5f, (b0.y + b1.y) * 0.5f - s * 0.025f }, b1 };
        StrokeOpt o; o.alpha = 0.92f;
        I.stroke(br, 3, lwMain * 1.5f, o);
        break;
      }
      case 3: {  // headband: a solid strip across the forehead
        Vec2 bp[10];
        int n = 0;
        static const float lons[5] = { -72, -36, 0, 36, 72 };
        for (int i = 0; i < 5; i++) bp[n++] = g.anchor(lons[i], -18, 1.02f);
        for (int i = 4; i >= 0; i--) bp[n++] = g.anchor(lons[i], -31, 1.02f);
        P96 strip;
        chaikin(strip, bp, n, true, 1);
        I.pencilFill(strip.p, strip.n, 0.92f, s);
        closedOutline(strip, lwThin, 0);
        break;
      }
      case 4: {  // beret, slouching to one side
        float sd = (float)f.brimSide;
        Vec2 c = g.anchor(sd * 20, -52, 1.0f);
        P96 b;
        blobPts(b, R, c.x + sd * g.w * 0.12f, c.y - s * 0.02f, g.w * 0.95f, s * 0.3f, 0.1f);
        I.scanFill(b.p, b.n, I.paper, 0.8f, 1.0f, 0.05f);
        if (colored) I.scanFill(b.p, b.n, accent, 0.72f, 1.3f, 0.18f);
        else toneFill(b, f.idx[C_HAIRINK] == 4 ? 1 : f.idx[C_HAIRINK], s);
        closedOutline(b, lwMain * 0.8f);
        Vec2 stem = g.anchor(sd * 8, -64, 1.1f);
        I.dot(stem.x, stem.y, lwThin * 1.2f, I.ink, 0.9f);
        break;
      }
      case 5: {  // turban: wrapped dome with a peak
        P96 dome;
        Vec2 dp[8];
        int n = 0;
        for (int i = 0; i < 5; i++) dp[n++] = hairline5[i];
        dp[n].x = g.w * 0.9f; dp[n].y = -s * f.skullY * 0.68f; n++;
        dp[n].x = f.turn * g.w * 0.2f + g.ts * g.w * 0.22f; dp[n].y = -s * (f.skullY + 0.3f); n++;   // the peak leans
        dp[n].x = -g.w * 0.9f; dp[n].y = -s * f.skullY * 0.7f; n++;
        chaikin(dome, dp, n, true, 1);
        I.scanFill(dome.p, dome.n, I.paper, 0.85f, 1.0f, 0.05f);
        if (colored) I.scanFill(dome.p, dome.n, accent, 0.68f, 1.2f, 0.16f);
        else { I.scanFill(dome.p, dome.n, I.ink, 0.06f); I.hatchFill(dome.p, dome.n, s * 0.07f, 0.5f, 0.3f, 1); }
        closedOutline(dome, lwMain * 0.85f);
        for (int k = 0; k < 3; k++) {  // diagonal wrap folds
          Vec2 a = g.anchor(-52 + k * 8.0f, f.hairLine - 2 - k * 11.0f, 1.05f);
          Vec2 b2 = g.anchor(50 - k * 4.0f, f.hairLine - 18 - k * 11.0f, 1.07f);
          Vec2 l[3] = { a, { (a.x + b2.x) * 0.5f, (a.y + b2.y) * 0.5f - s * 0.035f }, b2 };
          I.sline(l, 3, lwThin * 1.3f, 0.75f);
        }
        {  // the front V of the dastar
          Vec2 vc = g.anchor(0, f.hairLine + 6, 1.03f);
          Vec2 va = g.anchor(-46, f.hairLine - 10, 1.04f);
          Vec2 vb = g.anchor(46, f.hairLine - 10, 1.04f);
          Vec2 l1[2] = { va, vc };
          Vec2 l2[2] = { vb, vc };
          I.sline(l1, 2, lwThin * 1.4f, 0.85f);
          I.sline(l2, 2, lwThin * 1.4f, 0.85f);
        }
        P96 band; chaikin(band, hairline5, 5, false, 2);
        I.sline(band.p, band.n, lwMain * 0.9f, 0.85f);
        break;
      }
      case 6: {  // headscarf: dome + falls framing the face, knot below
        P96 dome;
        capPoly(dome, 1.12f, s * 0.06f);
        I.scanFill(dome.p, dome.n, I.paper, 0.85f, 1.0f, 0.05f);
        if (colored) I.scanFill(dome.p, dome.n, accent, 0.62f, 1.2f, 0.16f);
        else I.scanFill(dome.p, dome.n, I.ink, 0.07f);
        closedOutline(dome, lwMain * 0.8f);
        for (int sd = -1; sd <= 1; sd += 2) {
          float sw = g.swell(sd);
          Vec2 fpts[4] = { { sd * g.w * 0.8f * sw, -s * 0.45f },
                           { sd * g.w * 1.06f * sw, -s * 0.05f },
                           { sd * g.w * 1.0f * sw, s * f.chinY * 0.7f },
                           { sd * g.w * 0.62f * sw, s * f.chinY * 1.02f } };
          P96 sm; chaikin(sm, fpts, 4, false, 1);
          StrokeOpt o; o.alpha = 0.85f;
          I.stroke(sm.p, sm.n, lwMain * 0.9f, o);
        }
        Vec2 kc = { f.turn * g.w * 0.3f, s * f.chinY * 1.05f };
        wobEllipse(kc.x, kc.y, s * 0.03f, s * 0.025f, lwThin, 0.8f);
        break;
      }
      case 7: {  // flower crown over the hair
        int nf = R.ri(4, 6);
        for (int i = 0; i < nf; i++) {
          float t = (float)i / (nf - 1);
          Vec2 c = g.anchor((t * 2 - 1) * 62, f.hairLine - 4 + 3 * fsin(t * 7), 1.05f);
          RGB fc = f.accentCol >= 0 ? ACCENTC[(f.accentCol + i) % 6] : I.ink;
          float fr = s * R.rr(0.02f, 0.032f);
          for (int k = 0; k < 5; k++) {
            float a = (float)k / 5 * TAU + t;
            I.dot(c.x + fcos(a) * fr * 1.5f, c.y + fsin(a) * fr * 1.5f, fr * 0.8f, fc, 0.75f);
          }
          I.dot(c.x, c.y, fr * 0.7f, I.ink, 0.85f);
        }
        break;
      }
      case 8: {  // bandana: knotted band with tails
        Vec2 bp2[10];
        int n = 0;
        static const float lons2[5] = { -72, -36, 0, 36, 72 };
        for (int i = 0; i < 5; i++) bp2[n++] = g.anchor(lons2[i], -20, 1.02f);
        for (int i = 4; i >= 0; i--) bp2[n++] = g.anchor(lons2[i], -32, 1.02f);
        P96 strip;
        chaikin(strip, bp2, n, true, 1);
        if (colored) { I.scanFill(strip.p, strip.n, accent, 0.7f, 1.0f, 0.15f); closedOutline(strip, lwThin, 0); }
        else I.pencilFill(strip.p, strip.n, 0.9f, s);
        if (colored) for (int k = 0; k < 6; k++) {  // dot pattern
          Vec2 c = g.anchor(R.rr(-60, 60), R.rr(-30, -22), 1.03f);
          I.dot(c.x, c.y, s * 0.008f + 0.3f, I.paper, 0.8f);
        }
        float sd = -g.ts;
        float z;
        Vec2 e = g.anchor(sd * 78, -26, 1.04f, &z);
        if (z > -0.3f) {  // knot + tails
          wobEllipse(e.x, e.y, s * 0.028f, s * 0.024f, lwThin, 0.85f);
          for (int k = 0; k < 2; k++) {
            Vec2 t2[2] = { { e.x, e.y + s * 0.01f }, { e.x + sd * s * R.rr(0.04f, 0.07f), e.y + s * R.rr(0.06f, 0.1f) } };
            I.sline(t2, 2, lwThin * 1.2f, 0.8f);
          }
        }
        break;
      }
    }
  }

  // ------------------------------------------------ eyes / brows / glasses
  Vec2 eyeC[2]; float eyeR_[2]; float eyeFsh[2]; float eyeZ[2];

  void computeEyes() {
    for (int i = 0; i < 2; i++) {
      float sd = i == 0 ? -1 : 1;
      float z;
      Vec2 c = g.anchor(sd * f.eyeSpread, f.eyeLat + (i == 0 ? f.eyeDyL : f.eyeDyR) * 60, 0.98f, &z);
      eyeC[i] = c;
      eyeZ[i] = z;
      eyeFsh[i] = g.fshort(z);
      eyeR_[i] = s * 0.084f * f.eyeScale * (i == 0 ? f.eyeJitL : f.eyeJitR);
    }
  }

  void pupil(int i, float rx) {
    float pr = eyeR_[i] * R.rr(0.34f, 0.48f);
    float px = eyeC[i].x + f.gazeX * (rx - pr) * 1.2f;
    float py = eyeC[i].y + f.gazeY * (eyeR_[i] - pr);
    RGB c = I.ink;
    if (f.heterochromia && i == (f.patchSide > 0 ? 1 : 0) && f.accentCol >= 0) c = ACCENTC[f.accentCol];
    I.dot(px, py, pr < 0.9f ? 0.9f : pr, c, 0.95f);
  }

  void eyes() {
    computeEyes();
    int type = f.idx[C_EYES];
    for (int i = 0; i < 2; i++) {
      if (eyeZ[i] < 0.08f || eyeHide[i]) continue;  // behind the head, or under a fringe
      float rx = eyeR_[i] * eyeFsh[i], ry = eyeR_[i];
      int t = type;
      if (type == 5) t = (i == (f.patchSide > 0 ? 1 : 0)) ? 2 : 0;  // wink
      switch (t) {
        case 0:  // round: an arc or two and a pupil
          wobEllipse(eyeC[i].x, eyeC[i].y, rx, ry, lwThin * 1.5f, 0.95f);
          pupil(i, rx);
          break;
        case 1:  // dot
          I.dot(eyeC[i].x, eyeC[i].y, (ry * 0.5f) < 1.0f ? 1.0f : ry * 0.5f, I.ink, 0.95f);
          break;
        case 2: {  // sleepy line
          Vec2 l[3] = { { eyeC[i].x - rx, eyeC[i].y }, { eyeC[i].x, eyeC[i].y + ry * 0.18f }, { eyeC[i].x + rx, eyeC[i].y } };
          I.sline(l, 3, lwThin * 1.4f, 0.9f);
          if (R.chance(0.5f)) {
            Vec2 l2[2] = { { eyeC[i].x - rx * 0.5f, eyeC[i].y + ry * 0.55f }, { eyeC[i].x + rx * 0.55f, eyeC[i].y + ry * 0.5f } };
            I.sline(l2, 2, lwThin, 0.5f);
          }
          break;
        }
        case 3:  // big with a lid line
          wobEllipse(eyeC[i].x, eyeC[i].y, rx * 1.32f, ry * 1.32f, lwThin * 1.2f, 0.9f);
          pupil(i, rx * 1.1f);
          { P96 lid; arcPts(lid, eyeC[i].x, eyeC[i].y, rx * 1.45f, ry * 1.45f, 3.5f, 5.9f, 8);
            if (R.chance(0.6f)) I.sline(lid.p, lid.n, lwThin, 0.6f); }
          break;
        case 4:  // oval (tall)
          wobEllipse(eyeC[i].x, eyeC[i].y, rx * 0.75f, ry * 1.25f, lwThin * 1.2f, 0.9f);
          pupil(i, rx * 0.6f);
          break;
        case 6: {  // glad: closed happy arc
          P96 a; arcPts(a, eyeC[i].x, eyeC[i].y + ry * 0.3f, rx, ry * 0.8f, 3.5f, 5.9f, 8);
          I.sline(a.p, a.n, lwThin * 1.5f, 0.9f);
          break;
        }
      }
      if (f.lashes && t != 2) {  // little lash ticks, top-outer rim
        float lrx = rx * (t == 3 ? 1.4f : 1.05f), lry = ry * (t == 3 ? 1.4f : 1.05f);
        float sd = i == 0 ? -1 : 1;
        for (int k = 0; k < 3; k++) {
          float a = -TAU * 0.25f + sd * (0.5f + 0.35f * k);
          Vec2 l[2] = { { eyeC[i].x + fcos(a) * lrx, eyeC[i].y + fsin(a) * lry },
                        { eyeC[i].x + fcos(a) * (lrx + s * 0.028f), eyeC[i].y + fsin(a) * (lry + s * 0.028f) } };
          I.sline(l, 2, lwThin, 0.8f);
        }
      }
    }
  }

  void brows() {
    int type = f.idx[C_BROWS];
    if (type == 0) return;
    float raise = (f.idx[C_EXPR] == 3) ? 0.05f : 0.0f;  // surprised
    for (int i = 0; i < 2; i++) {
      if ((eyeZ[i] < 0.08f || eyeHide[i]) && type != 6) continue;
      float sd = i == 0 ? -1 : 1;
      float lift = (i == 0 ? f.browRL : f.browRR) + raise;
      float tilt = (i == 0 ? f.browTL : f.browTR) * -sd;
      float by = eyeC[i].y - eyeR_[i] * 1.6f - (lift + 0.035f) * s;
      float bw = eyeR_[i] * R.rr(1.3f, 1.7f) * eyeFsh[i];
      switch (type) {
        case 3: {  // comma
          P96 a; arcPts(a, eyeC[i].x, by + bw * 0.3f, bw, bw * 0.75f, 3.6f, 5.4f, 7);
          StrokeOpt o; o.wedge = true; o.alpha = 0.85f;
          I.stroke(a.p, a.n, lwMain * 0.9f, o);
          break;
        }
        case 2: {  // thick
          Vec2 l[2] = { { eyeC[i].x - bw, by + tilt * bw }, { eyeC[i].x + bw, by - tilt * bw } };
          StrokeOpt o; o.alpha = 0.9f;
          I.stroke(l, 2, lwMain * 1.5f, o);
          break;
        }
        case 6: {  // unibrow
          if (i == 1) break;
          Vec2 l[3] = { { eyeC[0].x - bw, by }, { (eyeC[0].x + eyeC[1].x) * 0.5f, by - s * 0.015f }, { eyeC[1].x + bw, by } };
          StrokeOpt o; o.alpha = 0.9f;
          I.stroke(l, 3, lwMain * 1.3f, o);
          break;
        }
        case 4: {  // angled
          Vec2 l[2] = { { eyeC[i].x - bw, by + (0.18f + tilt) * bw * sd }, { eyeC[i].x + bw, by - (0.18f + tilt) * bw * sd } };
          StrokeOpt o; o.alpha = 0.9f;
          I.stroke(l, 2, lwMain * 0.8f, o);
          break;
        }
        default: {  // line / high
          float hy = type == 5 ? by - s * 0.05f : by;
          Vec2 l[2] = { { eyeC[i].x - bw, hy + tilt * bw }, { eyeC[i].x + bw, hy - tilt * bw } };
          StrokeOpt o; o.alpha = 0.88f;
          I.stroke(l, 2, lwMain * 0.85f, o);
          break;
        }
      }
    }
  }

  void eyewear() {
    int type = f.idx[C_EYEW];
    if (type == 0) return;
    float lw = lwThin * 1.3f;
    if (type == 5) {  // eyepatch
      int i = f.patchSide > 0 ? 1 : 0;
      if (eyeZ[i] < 0.08f) i = 1 - i;
      float r = eyeR_[i] * 1.9f;
      P96 b;
      blobPts(b, R, eyeC[i].x, eyeC[i].y, r * eyeFsh[i], r, 0.08f);
      I.pencilFill(b.p, b.n, 0.95f, r * 2);
      closedOutline(b, lwThin, 0);
      float sd = i == 0 ? -1 : 1;
      Vec2 s1[3] = { { eyeC[i].x - sd * r * 0.7f, eyeC[i].y - r * 0.5f },
                     g.anchor(-sd * 40, -26, 1.0f), g.anchor(-sd * 80, -14, 1.0f) };
      I.sline(s1, 3, lwThin, 0.85f);
      Vec2 s2[2] = { { eyeC[i].x + sd * r * 0.6f, eyeC[i].y - r * 0.6f }, g.anchor(sd * 80, -32, 1.0f) };
      I.sline(s2, 2, lwThin, 0.85f);
      return;
    }
    if (type == 4) {  // monocle
      int i = f.patchSide > 0 ? 1 : 0;
      if (eyeZ[i] < 0.08f) i = 1 - i;
      wobEllipse(eyeC[i].x, eyeC[i].y, eyeR_[i] * 1.7f * eyeFsh[i], eyeR_[i] * 1.7f, lw, 0.9f);
      Vec2 dl[3] = { { eyeC[i].x, eyeC[i].y + eyeR_[i] * 1.8f },
                     { eyeC[i].x + s * 0.02f, eyeC[i].y + s * 0.16f },
                     { eyeC[i].x - s * 0.015f, eyeC[i].y + s * 0.24f } };
      I.sline(dl, 3, lwThin * 0.8f, 0.6f);
      return;
    }
    // two lenses + bridge + temples
    float lr[2];
    for (int i = 0; i < 2; i++) lr[i] = eyeR_[i] * R.rr(1.9f, 2.3f);
    for (int i = 0; i < 2; i++) {
      if (eyeZ[i] < 0.05f) continue;
      float rx = lr[i] * eyeFsh[i], ry = lr[i];
      if (type == 1) wobEllipse(eyeC[i].x, eyeC[i].y, rx, ry, lw, 0.9f);
      else if (type == 2) {  // square-ish
        Vec2 q[5] = { { eyeC[i].x - rx, eyeC[i].y - ry * 0.8f }, { eyeC[i].x + rx, eyeC[i].y - ry * 0.8f },
                      { eyeC[i].x + rx, eyeC[i].y + ry * 0.8f }, { eyeC[i].x - rx, eyeC[i].y + ry * 0.8f },
                      { eyeC[i].x - rx, eyeC[i].y - ry * 0.8f } };
        P96 sm; chaikin(sm, q, 5, false, 1);
        I.sline(sm.p, sm.n, lw, 0.9f);
      } else {  // shades
        P96 b;
        blobPts(b, R, eyeC[i].x, eyeC[i].y, rx, ry * 0.95f, 0.06f);
        I.pencilFill(b.p, b.n, 0.92f, rx * 2);
        closedOutline(b, lwThin, 0);
      }
    }
    if (eyeZ[0] > 0.05f && eyeZ[1] > 0.05f) {
      Vec2 br[3] = { { eyeC[0].x + lr[0] * eyeFsh[0] * 0.9f, eyeC[0].y - lr[0] * 0.25f },
                     { (eyeC[0].x + eyeC[1].x) * 0.5f, (eyeC[0].y + eyeC[1].y) * 0.5f - lr[0] * 0.5f },
                     { eyeC[1].x - lr[1] * eyeFsh[1] * 0.9f, eyeC[1].y - lr[1] * 0.25f } };
      I.sline(br, 3, lw, 0.85f);
    }
    for (int i = 0; i < 2; i++) {  // temple arms toward the ears
      if (eyeZ[i] < 0.05f) continue;
      float sd = i == 0 ? -1 : 1;
      float ez;
      Vec2 ear = g.anchor(sd * 82, f.eyeLat, 1.0f, &ez);
      if (ez < -0.1f) continue;
      ear.x *= 1 + (sd == (int)g.ts ? 0.1f : -0.28f) * g.at;
      Vec2 t2[2] = { { eyeC[i].x + sd * lr[i] * eyeFsh[i], eyeC[i].y - lr[i] * 0.1f }, { ear.x, ear.y - s * 0.015f } };
      I.sline(t2, 2, lwThin, 0.75f);
    }
  }

  // ------------------------------------------------ nose & mouth
  void nose() {
    int type = f.idx[C_NOSE];
    float ts = g.ts;
    float len = f.noseLen * (type == 5 ? 0.6f : type == 4 ? 1.15f : 1.0f);
    float latTip = 12 + 13 * len;
    Vec2 B = g.anchor(0, f.eyeLat - 2, 0.97f);
    Vec2 M = g.anchor(-g.turn * 6, (f.eyeLat + latTip) * 0.5f, 1.06f + 0.1f * g.at);
    // turned heads: the nose clears the silhouette, like the reference profiles
    Vec2 T = g.anchor(0, latTip, 1.02f + 0.16f * len * (type == 1 ? 1.15f : 1.0f) + 0.24f * g.at);
    float hook = s * 0.062f * f.noseHook * (type == 1 ? 1.6f : type == 4 ? 1.3f : 1.0f);
    Vec2 H = { T.x - ts * hook * (type == 1 ? 1.5f : 1.1f), T.y + hook * 0.12f };
    StrokeOpt o; o.alpha = 0.95f; o.over = s * 0.015f;

    switch (type) {
      case 2: {  // button: little circle at the tip
        Vec2 pts[3] = { B, M, { T.x, T.y - s * 0.03f } };
        P96 sm; chaikin(sm, pts, 3, false, 2);
        I.stroke(sm.p, sm.n, lwMain * 0.95f, o);
        wobEllipse(T.x - ts * s * 0.012f, T.y, s * 0.035f, s * 0.03f, lwThin * 1.3f, 0.85f);
        break;
      }
      case 3: {  // triangle wedge
        Vec2 pts[3] = { B, T, { T.x - ts * s * 0.09f, T.y - s * 0.01f } };
        I.stroke(pts, 2, lwMain * 0.85f, o);
        Vec2 base[2] = { T, pts[2] };
        I.stroke(base, 2, lwMain * 0.8f, o);
        break;
      }
      default: {  // long / hook / big / tiny: one curve around the tip
        Vec2 pts[4] = { B, M, T, H };
        P96 sm; chaikin(sm, pts, 4, false, 2);
        I.stroke(sm.p, sm.n, lwMain * (type == 4 ? 1.25f : 1.0f), o);
        if (type == 4 && R.chance(0.7f)) {  // big: hint of the far side
          Vec2 pts2[2] = { { B.x + ts * s * 0.05f, B.y + s * 0.06f }, { T.x + ts * s * 0.035f, T.y - s * 0.045f } };
          I.sline(pts2, 2, lwThin, 0.5f);
        }
        break;
      }
    }
    if (f.nostril && type != 3) {
      P96 a;
      arcPts(a, H.x - ts * s * 0.02f, H.y + s * 0.005f, s * 0.022f, s * 0.018f, 1.2f, 3.6f, 6);
      I.sline(a.p, a.n, lwThin, 0.7f);
    }
  }

  void mouth() {
    int type = f.idx[C_MOUTH];
    RGB keepInk = I.ink;
    float keepT = lwThin, keepM = lwMain;
    if (f.lipCol >= 0) { I.ink = LIP_C; lwThin *= 1.3f; lwMain *= 1.15f; }
    float z;
    Vec2 Mc = g.anchor(0, 50, 0.92f, &z);
    Mc.x += f.mouthShift * s * 0.2f;
    float mw = s * 0.15f * f.mouthW * g.fshort(z);
    float curve = f.mouthCurve * s * 0.07f;
    switch (type) {
      case 1: {  // smile
        Vec2 l[3] = { { Mc.x - mw, Mc.y - curve * 0.2f }, { Mc.x, Mc.y + fabsf(curve) + s * 0.02f }, { Mc.x + mw, Mc.y - curve * 0.2f } };
        P96 sm; chaikin(sm, l, 3, false, 2);
        StrokeOpt o; o.alpha = 0.93f;
        I.stroke(sm.p, sm.n, lwThin * 2.2f, o);
        break;
      }
      case 2: {  // frown
        Vec2 l[3] = { { Mc.x - mw, Mc.y + s * 0.02f }, { Mc.x, Mc.y - fabsf(curve) - s * 0.025f }, { Mc.x + mw, Mc.y + s * 0.02f } };
        P96 sm; chaikin(sm, l, 3, false, 2);
        StrokeOpt o; o.alpha = 0.93f;
        I.stroke(sm.p, sm.n, lwThin * 2.2f, o);
        break;
      }
      case 3: {  // open
        P96 b;
        blobPts(b, R, Mc.x, Mc.y + s * 0.015f, mw * 0.75f, mw * (f.mouthCurve > 0 ? 0.62f : 0.5f), 0.12f);
        I.pencilFill(b.p, b.n, 0.9f, mw);
        closedOutline(b, lwThin * 1.2f, 0);
        if (f.teeth) {
          Vec2 t2[2] = { { Mc.x - mw * 0.5f, Mc.y - mw * 0.12f }, { Mc.x + mw * 0.5f, Mc.y - mw * 0.15f } };
          RGB pc = I.paper;
          I.sline(t2, 2, lwThin * 1.6f, 0.85f, &pc);
        }
        break;
      }
      case 4:  // o
        wobEllipse(Mc.x, Mc.y + s * 0.01f, mw * 0.42f, mw * 0.5f, lwThin * 1.6f, 0.92f);
        break;
      case 5: {  // zigzag
        P48 l;
        int nz = 4;
        for (int i = 0; i <= nz * 2; i++) {
          float t = (float)i / (nz * 2);
          l.add(Mc.x - mw + 2 * mw * t, Mc.y + ((i & 1) ? -1 : 1) * s * 0.026f + curve * (0.5f - fabsf(t - 0.5f)));
        }
        I.sline(l.p, l.n, lwThin * 1.3f, 0.9f);
        break;
      }
      case 6: {  // smirk
        float sd = f.mouthShift >= 0 ? 1 : -1;
        Vec2 l[3] = { { Mc.x - mw * 0.9f, Mc.y + s * 0.012f }, { Mc.x + mw * 0.3f * sd, Mc.y + curve }, { Mc.x + mw * 0.95f, Mc.y - s * 0.03f * sd } };
        P96 sm; chaikin(sm, l, 3, false, 2);
        StrokeOpt o; o.alpha = 0.92f;
        I.stroke(sm.p, sm.n, lwThin * 2.0f, o);
        if (R.chance(0.5f)) {
          Vec2 c2[2] = { { Mc.x + mw * 1.05f, Mc.y - s * 0.05f * sd }, { Mc.x + mw * 1.15f, Mc.y + s * 0.005f } };
          I.sline(c2, 2, lwThin, 0.6f);
        }
        break;
      }
      default: {  // line
        Vec2 l[3] = { { Mc.x - mw, Mc.y + curve * 0.4f }, { Mc.x + f.mouthShift * mw * 0.3f, Mc.y - curve * 0.6f }, { Mc.x + mw, Mc.y + curve * 0.4f } };
        StrokeOpt o; o.alpha = 0.93f;
        I.stroke(l, 3, lwThin * 2.2f, o);
        break;
      }
    }
    I.ink = keepInk; lwThin = keepT; lwMain = keepM;
  }

  void facialHair() {
    int type = f.idx[C_FHAIR];
    if (type == 0) return;
    float z;
    Vec2 Mc = g.anchor(0, 40, 0.92f, &z);
    Vec2 Nt = g.anchor(0, 34, 0.96f);
    float mw = s * 0.1f * g.fshort(z);
    switch (type) {
      case 1: {  // moustache
        int v = R.ri(0, 2);
        if (v == 0) {
          Vec2 l[3] = { { Nt.x - mw, Nt.y + s * 0.03f }, { Nt.x, Nt.y + s * 0.008f }, { Nt.x + mw, Nt.y + s * 0.03f } };
          StrokeOpt o; o.alpha = 0.92f;
          I.stroke(l, 3, lwMain * 1.25f, o);
        } else if (v == 1) {
          for (int sd = -1; sd <= 1; sd += 2) {  // handlebar curls
            P96 a; arcPts(a, Nt.x + sd * mw * 0.75f, Nt.y + s * 0.03f, mw * 0.5f, s * 0.035f,
                          sd > 0 ? 3.6f : 5.2f, sd > 0 ? 5.8f : 3.0f, 8);
            StrokeOpt o; o.wedge = true; o.alpha = 0.9f;
            I.stroke(a.p, a.n, lwMain, o);
          }
        } else {  // pencil line
          Vec2 l[2] = { { Nt.x - mw * 0.8f, Nt.y + s * 0.035f }, { Nt.x + mw * 0.8f, Nt.y + s * 0.03f } };
          I.sline(l, 2, lwThin * 1.5f, 0.85f);
        }
        break;
      }
      case 2: {  // chin beard
        Vec2 ch = { f.turn * g.w * 0.3f, s * f.chinY * 0.9f };
        P96 b; blobPts(b, R, ch.x, ch.y, g.w * 0.32f * f.chinW + s * 0.05f, s * 0.09f, 0.15f);
        toneFill(b, f.idx[C_HAIRINK] == 4 ? 2 : f.idx[C_HAIRINK], s * 0.5f);
        closedOutline(b, lwThin, 0);
        break;
      }
      case 3: {  // stubble
        P96 jawp;
        Vec2 jp[7];
        int n = 0;
        for (int i = 0; i < 4; i++) jp[n++] = g.anchor(-60 + 40.0f * i, 46 - 8 * ((i == 1 || i == 2) ? 1 : 0), 0.98f);
        jp[n++] = g.anchor(40, 20, 0.9f);
        jp[n++] = g.anchor(0, 26, 0.85f);
        jp[n++] = g.anchor(-40, 20, 0.9f);
        chaikin(jawp, jp, n, true, 1);
        I.stippleFill(jawp.p, jawp.n, s * 0.032f, 0.4f);
        break;
      }
      case 4: {  // full beard: a fringe hugging the jaw
        P96 bp;
        Vec2 jp[9];
        int n = 0;
        jp[n++] = g.anchor(-76, 18, 1.02f);
        jp[n++] = g.anchor(-56, 46, 1.06f);
        jp[n++] = g.anchor(-26, 62, 1.08f);
        jp[n++] = g.anchor(26, 62, 1.08f);
        jp[n++] = g.anchor(56, 46, 1.06f);
        jp[n++] = g.anchor(76, 18, 1.02f);
        jp[n++] = g.anchor(44, 32, 0.88f);
        jp[n++] = g.anchor(0, 42, 0.84f);
        jp[n++] = g.anchor(-44, 32, 0.88f);
        chaikin(bp, jp, n, true, 1);
        int bink = f.idx[C_HAIRINK];
        bink = bink == 1 ? 2 : bink == 4 ? 3 : bink;  // hatch reads as teeth, light as nothing
        toneFill(bp, bink, s);
        closedOutline(bp, lwThin * 1.3f, s * 0.04f);
        break;
      }
      case 5: {  // goatee
        Vec2 ch = { f.turn * g.w * 0.3f, s * f.chinY * 0.85f };
        P96 b; blobPts(b, R, ch.x, ch.y, s * 0.05f, s * 0.07f, 0.18f);
        I.pencilFill(b.p, b.n, 0.85f, s * 0.1f);
        break;
      }
    }
  }

  // ------------------------------------------------ quirks
  void quirks() {
    if (f.blush && f.skinCol >= 0) {
      for (int i = 0; i < 2; i++) {
        float sd = i == 0 ? -1 : 1;
        float z;
        Vec2 c = g.anchor(sd * 42, 24, 0.95f, &z);
        if (z < 0.05f) continue;
        I.dot(c.x, c.y, s * 0.055f, BLUSH_C, 0.3f);
      }
    }
    if (f.freckles) {
      for (int i = 0; i < 2; i++) {
        float sd = i == 0 ? -1 : 1;
        float z;
        Vec2 c = g.anchor(sd * 38, 22, 0.95f, &z);
        if (z < 0.05f) continue;
        int nf = R.ri(3, 5);
        for (int k = 0; k < nf; k++)
          I.dot(c.x + R.rr(-1, 1) * s * 0.06f, c.y + R.rr(-1, 1) * s * 0.04f, s * 0.008f + 0.3f, I.ink, R.rr(0.3f, 0.55f));
      }
    }
    if (f.cheekLines) {
      float sd = -g.ts;
      float z;
      Vec2 c = g.anchor(sd * 48, 30, 0.95f, &z);
      if (z > 0.05f) {
        Vec2 l[2] = { { c.x - sd * s * 0.02f, c.y - s * 0.02f }, { c.x + sd * s * 0.035f, c.y + s * 0.03f } };
        I.sline(l, 2, lwThin, 0.5f);
        if (R.chance(0.5f)) {
          Vec2 l2[2] = { { l[0].x + sd * s * 0.035f, l[0].y + s * 0.01f }, { l[1].x + sd * s * 0.035f, l[1].y + s * 0.035f } };
          I.sline(l2, 2, lwThin, 0.4f);
        }
      }
    }
    if (f.mole) {
      float z;
      Vec2 c = g.anchor(R.rr(-50, 50), R.rr(10, 45), 0.96f, &z);
      if (z > 0.05f) I.dot(c.x, c.y, s * 0.012f + 0.4f, I.ink, 0.8f);
    }
    if (f.plaster) {
      float z;
      float lon = f.patchSide * R.rr(30, 55);
      Vec2 c = g.anchor(lon, R.rr(-20, 25), 0.97f, &z);
      if (z > 0.1f) {
        float a = R.rr(0, TAU), ca = fcos(a), sa = fsin(a);
        float L = s * 0.07f, W = s * 0.028f;
        for (int sd = -1; sd <= 1; sd += 2) {
          Vec2 l[2] = { { c.x - ca * L + sa * W * sd, c.y - sa * L - ca * W * sd },
                        { c.x + ca * L + sa * W * sd, c.y + sa * L - ca * W * sd } };
          I.sline(l, 2, lwThin, 0.7f);
        }
        for (int k = -1; k <= 1; k += 2) {
          Vec2 l[2] = { { c.x + ca * L * 0.55f * k + sa * W, c.y + sa * L * 0.55f * k - ca * W },
                        { c.x + ca * L * 0.55f * k - sa * W, c.y + sa * L * 0.55f * k + ca * W } };
          I.sline(l, 2, lwThin * 0.8f, 0.6f);
        }
      }
    }
    if (f.wrinkles) {
      for (int k = 0; k < 2; k++) {  // forehead lines
        Vec2 a = g.anchor(-28, f.hairLine * 0.55f + k * 7, 1.0f);
        Vec2 b2 = g.anchor(28, f.hairLine * 0.55f + k * 7 - 2, 1.0f);
        Vec2 m2 = { (a.x + b2.x) * 0.5f, (a.y + b2.y) * 0.5f - s * 0.015f };
        Vec2 l[3] = { a, m2, b2 };
        I.sline(l, 3, lwThin, 0.45f);
      }
      for (int i = 0; i < 2; i++) {  // crow's feet
        if (eyeZ[i] < 0.08f || eyeHide[i]) continue;
        float sd = i == 0 ? -1 : 1;
        for (int k = 0; k < 2; k++) {
          Vec2 p0 = { eyeC[i].x + sd * eyeR_[i] * 1.5f, eyeC[i].y - s * 0.01f + k * s * 0.025f };
          Vec2 l[2] = { p0, { p0.x + sd * s * 0.035f, p0.y + (k == 0 ? -1 : 1) * s * 0.015f } };
          I.sline(l, 2, lwThin * 0.9f, 0.45f);
        }
      }
    }
    if (f.bindi) {
      Vec2 c = g.anchor(0, f.eyeLat - 11, 0.99f);
      RGB bc = f.skinCol >= 0 ? ACCENTC[0] : I.ink;
      I.dot(c.x, c.y, s * 0.016f + 0.4f, bc, 0.9f);
    }
    if (f.nosering) {
      Vec2 c = g.anchor(-g.ts * 7, 27, 1.03f);
      wobEllipse(c.x - g.ts * s * 0.008f, c.y + s * 0.012f, s * 0.012f + 0.4f, s * 0.012f + 0.4f, lwThin * 0.9f, 0.9f);
    }
    if (f.piercings) {
      float sd = g.ts;
      float z;
      Vec2 e = g.anchor(sd * 88, 4, 1.0f, &z);
      if (z > -0.15f) {  // rings up the near ear
        e.x *= (1 + 0.1f * g.at) * 0.97f;
        for (int k = 0; k < 2; k++)
          wobEllipse(e.x + sd * s * 0.02f, e.y - s * (0.045f - 0.028f * k), s * 0.009f + 0.4f, s * 0.009f + 0.4f, lwThin * 0.8f, 0.85f);
      }
      if (R.chance(0.5f)) {  // brow stud
        int i = f.patchSide > 0 ? 1 : 0;
        if (eyeZ[i] > 0.08f && !eyeHide[i])
          I.dot(eyeC[i].x + (i == 0 ? -1 : 1) * eyeR_[i] * 1.3f, eyeC[i].y - eyeR_[i] * 1.9f, s * 0.01f + 0.3f, I.ink, 0.8f);
      }
    }
    if (f.facepaint) {
      RGB pc = f.accentCol >= 0 ? ACCENTC[f.accentCol] : I.ink;
      for (int i = 0; i < 2; i++) {  // dots under the eyes
        if (eyeZ[i] < 0.08f || eyeHide[i]) continue;
        for (int k = 0; k < 3; k++)
          I.dot(eyeC[i].x + (k - 1) * s * 0.035f, eyeC[i].y + eyeR_[i] * 1.9f + fabsf((float)k - 1) * s * 0.008f,
                s * 0.011f + 0.3f, pc, 0.8f);
      }
      if (R.chance(0.6f)) {  // chin mark
        Vec2 c = g.anchor(0, 60, 0.98f);
        Vec2 l[2] = { { c.x, c.y - s * 0.025f }, { c.x, c.y + s * 0.03f } };
        I.sline(l, 2, lwThin * 1.2f, 0.7f);
      }
      if (R.chance(0.5f)) {  // temple stripes
        float sd = -g.ts;
        float z;
        Vec2 c = g.anchor(sd * 56, -8, 1.0f, &z);
        if (z > 0) for (int k = 0; k < 2; k++) {
          Vec2 l[2] = { { c.x - s * 0.03f, c.y + k * s * 0.03f }, { c.x + s * 0.035f, c.y + k * s * 0.03f + s * 0.006f } };
          I.sline(l, 2, lwThin * 1.1f, 0.7f, f.accentCol >= 0 ? &pc : nullptr);
        }
      }
    }
    if (f.flowerEar) {
      float sd = g.ts;
      float z;
      Vec2 e = g.anchor(sd * 80, -12, 1.05f, &z);
      if (z > -0.2f) {
        RGB fc = f.accentCol >= 0 ? ACCENTC[f.accentCol] : I.ink;
        float fr = s * 0.024f;
        for (int k = 0; k < 5; k++) {
          float a = (float)k / 5 * TAU;
          I.dot(e.x + fcos(a) * fr * 1.4f, e.y + fsin(a) * fr * 1.4f, fr * 0.8f, fc, 0.8f);
        }
        I.dot(e.x, e.y, fr * 0.6f, I.ink, 0.85f);
      }
    }
    if (f.feather) {
      float sd = -g.ts;
      Vec2 base = { sd * g.w * 0.5f, -s * f.skullY * 0.82f };
      Vec2 tip = { base.x + sd * s * 0.16f, base.y - s * 0.38f };
      Vec2 spine[2] = { base, tip };
      I.sline(spine, 2, lwThin * 1.2f, 0.85f);
      for (int sd2 = -1; sd2 <= 1; sd2 += 2) {  // the vane
        Vec2 edge[3] = { { base.x + sd2 * s * 0.008f, base.y },
                         { (base.x + tip.x) * 0.5f + sd2 * s * 0.042f, (base.y + tip.y) * 0.5f },
                         tip };
        P96 sm; chaikin(sm, edge, 3, false, 1);
        I.sline(sm.p, sm.n, lwThin * 1.1f, 0.8f);
      }
      for (int k = 1; k <= 3; k++) {  // barbs
        float t = (float)k / 4;
        Vec2 p0 = { lerpf(base.x, tip.x, t), lerpf(base.y, tip.y, t) };
        Vec2 l[2] = { p0, { p0.x - sd * s * 0.03f, p0.y + s * 0.03f } };
        I.sline(l, 2, lwThin * 0.8f, 0.55f);
      }
    }
  }

  // ------------------------------------------------ shoulders & jewellery
  void garmentAndNeck() {
    float cx = f.turn * g.w * 0.3f, cy = s * f.chinY * 0.97f;
    float nw = g.w * 0.18f, len = s * 0.13f;
    float sy = cy + len;
    int gt = f.garment;
    if (gt < 0) neck();
    else {
      for (int sd = -1; sd <= 1; sd += 2) {  // neck
        Vec2 l[2] = { { cx + sd * nw, cy - s * 0.04f }, { cx + sd * nw * 1.05f, cy + len } };
        I.sline(l, 2, lwThin * 1.3f, 0.8f);
      }
      float sw2 = g.w * R.rr(0.95f, 1.15f);
      float drop = s * R.rr(0.14f, 0.2f);
      if (gt == 4) { sw2 *= 1.05f; drop *= 1.45f; }
      if (f.accentCol >= 0 && R.chance(0.65f)) {  // a hint of colour on the shoulders
        Vec2 wp[6] = { { cx - nw * 1.3f, sy + s * 0.02f }, { cx - sw2, sy + drop },
                       { cx - sw2 * 1.02f, sy + drop + s * 0.3f }, { cx + sw2 * 1.02f, sy + drop + s * 0.3f },
                       { cx + sw2, sy + drop }, { cx + nw * 1.3f, sy + s * 0.02f } };
        P96 sm; chaikin(sm, wp, 6, true, 1);
        I.scanFill(sm.p, sm.n, ACCENTC[f.accentCol], 0.3f / I.boost, 1.5f, 0.25f);
      }
      StrokeOpt so; so.alpha = 0.85f;
      if (gt != 4) {  // shoulder slopes
        for (int sd = -1; sd <= 1; sd += 2) {
          Vec2 sh[3] = { { cx + sd * nw * 1.15f, sy }, { cx + sd * sw2 * 0.55f, sy + drop * 0.5f }, { cx + sd * sw2, sy + drop } };
          P96 sm; chaikin(sm, sh, 3, false, 1);
          I.stroke(sm.p, sm.n, lwMain * 0.75f, so);
        }
      }
      switch (gt) {
        case 0: {  // crew neck
          Vec2 c2[3] = { { cx - nw * 1.3f, sy }, { cx, sy + s * 0.045f }, { cx + nw * 1.3f, sy } };
          I.sline(c2, 3, lwThin * 1.4f, 0.85f);
          break;
        }
        case 1: {  // hoodie
          for (int sd = -1; sd <= 1; sd += 2) {
            P96 a; arcPts(a, cx + sd * nw * 1.9f, sy - s * 0.015f, s * 0.075f, s * 0.06f,
                          sd > 0 ? 4.2f : 5.2f, sd > 0 ? 7.2f : 8.2f, 8);
            I.sline(a.p, a.n, lwThin * 1.4f, 0.8f);
          }
          for (int sd = -1; sd <= 1; sd += 2) {  // drawstrings
            Vec2 l[2] = { { cx + sd * nw * 0.5f, sy + s * 0.01f }, { cx + sd * nw * 0.6f, sy + s * 0.1f } };
            I.sline(l, 2, lwThin, 0.7f);
            I.dot(l[1].x, l[1].y, s * 0.008f + 0.3f, I.ink, 0.8f);
          }
          break;
        }
        case 2: {  // jacket: lapels + studs
          for (int sd = -1; sd <= 1; sd += 2) {
            Vec2 l[2] = { { cx + sd * nw * 1.05f, sy - s * 0.005f }, { cx + sd * nw * 2.0f, sy + s * 0.13f } };
            I.sline(l, 2, lwThin * 1.5f, 0.88f);
            Vec2 l2[2] = { { cx + sd * nw * 1.05f, sy - s * 0.005f }, { cx + sd * nw * 0.35f, sy + s * 0.12f } };
            I.sline(l2, 2, lwThin * 1.3f, 0.8f);
          }
          for (int k = 0; k < 3; k++)
            for (int sd = -1; sd <= 1; sd += 2)
              I.dot(cx + sd * (nw * 2.2f + k * s * 0.055f), sy + drop * (0.35f + 0.18f * k), s * 0.009f + 0.35f, I.ink, 0.85f);
          break;
        }
        case 3: {  // tee: scoop
          Vec2 c2[3] = { { cx - nw * 1.2f, sy }, { cx, sy + s * 0.06f }, { cx + nw * 1.2f, sy } };
          P96 sm; chaikin(sm, c2, 3, false, 1);
          I.sline(sm.p, sm.n, lwThin * 1.3f, 0.8f);
          break;
        }
        case 4: {  // poncho: one big triangle with a zigzag hem
          Vec2 tri[3] = { { cx, sy - s * 0.02f }, { cx - sw2, sy + drop }, { cx + sw2, sy + drop } };
          Vec2 l1[2] = { tri[0], tri[1] };
          Vec2 l2[2] = { tri[0], tri[2] };
          I.stroke(l1, 2, lwMain * 0.8f, so);
          I.stroke(l2, 2, lwMain * 0.8f, so);
          P48 hem;
          for (int i = 0; i <= 6; i++) {
            float t = (float)i / 6;
            hem.add(lerpf(tri[1].x + s * 0.03f, tri[2].x - s * 0.03f, t), tri[1].y + ((i & 1) ? s * 0.035f : 0));
          }
          I.sline(hem.p, hem.n, lwThin * 1.2f, 0.75f);
          Vec2 st[2] = { { (tri[0].x + tri[1].x) * 0.5f, (tri[0].y + tri[1].y) * 0.5f + s * 0.02f },
                         { (tri[0].x + tri[2].x) * 0.5f, (tri[0].y + tri[2].y) * 0.5f + s * 0.02f } };
          I.sline(st, 2, lwThin, 0.5f);
          break;
        }
        case 5: {  // shawl: wrap + fringe
          for (int sd = -1; sd <= 1; sd += 2) {
            Vec2 l[3] = { { cx + sd * nw * 1.1f, sy + s * 0.005f }, { cx + sd * sw2 * 0.5f, sy + drop * 0.42f }, { cx + sd * sw2 * 0.9f, sy + drop * 0.95f } };
            P96 sm; chaikin(sm, l, 3, false, 1);
            I.sline(sm.p, sm.n, lwMain * 0.7f, 0.8f);
            for (int k = 1; k <= 3; k++) {
              float t = (float)k / 4;
              Vec2 p0 = { lerpf(l[0].x, l[2].x, t), lerpf(l[0].y, l[2].y, t) + s * 0.02f };
              Vec2 tk[2] = { p0, { p0.x, p0.y + s * 0.035f } };
              I.sline(tk, 2, lwThin * 0.9f, 0.6f);
            }
          }
          I.dot(cx, sy + s * 0.05f, s * 0.012f + 0.4f, I.ink, 0.85f);
          break;
        }
        case 6: {  // band-collar shirt: placket + buttons
          Vec2 c2[3] = { { cx - nw * 1.1f, sy }, { cx, sy + s * 0.03f }, { cx + nw * 1.1f, sy } };
          I.sline(c2, 3, lwThin * 1.3f, 0.85f);
          Vec2 pl[2] = { { cx, sy + s * 0.035f }, { cx + s * 0.005f, sy + drop + s * 0.1f } };
          I.sline(pl, 2, lwThin, 0.7f);
          for (int k = 0; k < 2; k++) I.dot(cx + s * 0.012f, sy + s * (0.09f + 0.09f * k), s * 0.007f + 0.3f, I.ink, 0.8f);
          break;
        }
        case 7: {  // straps
          for (int sd = -1; sd <= 1; sd += 2) {
            Vec2 l[2] = { { cx + sd * nw * 1.6f, sy + s * 0.01f }, { cx + sd * nw * 2.1f, sy + drop * 0.7f } };
            I.sline(l, 2, lwThin * 1.3f, 0.8f);
          }
          Vec2 c2[3] = { { cx - nw * 1.6f, sy + s * 0.012f }, { cx, sy + s * 0.05f }, { cx + nw * 1.6f, sy + s * 0.012f } };
          I.sline(c2, 3, lwThin * 1.2f, 0.8f);
          break;
        }
      }
    }
    // jewellery sits over whatever is worn
    if (f.choker) {
      Vec2 l[3] = { { cx - nw * 1.05f, cy + len * 0.45f }, { cx, cy + len * 0.6f }, { cx + nw * 1.05f, cy + len * 0.45f } };
      I.sline(l, 3, lwThin * 2.0f, 0.92f);
      I.dot(cx, cy + len * 0.75f, s * 0.009f + 0.3f, I.ink, 0.85f);
    }
    if (f.beads) {
      RGB bc = f.accentCol >= 0 ? ACCENTC[f.accentCol] : I.ink;
      int nb = R.ri(7, 10);
      for (int k = 0; k < nb; k++) {
        float t = (float)k / (nb - 1);
        float bx = cx + (t * 2 - 1) * nw * 2.1f;
        float by = sy + s * 0.06f + (1 - fabsf(t * 2 - 1)) * s * 0.075f;
        I.dot(bx, by, s * R.rr(0.011f, 0.016f) + 0.3f, (k % 2) ? bc : I.ink, 0.8f);
      }
    }
    if (f.pendant) {
      Vec2 v1[3] = { { cx - nw * 1.2f, sy }, { cx, sy + s * 0.1f }, { cx + nw * 1.2f, sy } };
      I.sline(v1, 3, lwThin * 0.9f, 0.7f);
      float pr = s * 0.028f;
      float py = sy + s * 0.12f;
      wobEllipse(cx, py, pr, pr, lwThin, 0.85f);
      Vec2 pl1[2] = { { cx, py - pr }, { cx, py + pr } };
      I.sline(pl1, 2, lwThin * 0.8f, 0.7f);
      for (int sd = -1; sd <= 1; sd += 2) {
        Vec2 pl2[2] = { { cx, py }, { cx + sd * pr * 0.7f, py + pr * 0.7f } };
        I.sline(pl2, 2, lwThin * 0.8f, 0.7f);
      }
    }
  }

  // ------------------------------------------------ a little aura doodle
  void aura() {
    if (f.aura < 0) return;
    RGB c = f.accentCol >= 0 ? ACCENTC[f.accentCol] : (f.haloCol >= 0 ? HALOC[f.haloCol] : I.ink);
    float ax = -g.ts * g.w * R.rr(1.25f, 1.45f);
    float ay = -s * R.rr(0.55f, 0.85f);
    float a = s * 0.075f;
    float al = 0.65f;
    switch (f.aura) {
      case 0: {  // star
        Vec2 st[6];
        for (int k = 0; k < 6; k++) {
          float an = -TAU * 0.25f + (float)k * TAU * 0.4f;
          st[k].x = ax + fcos(an) * a; st[k].y = ay + fsin(an) * a;
        }
        I.sline(st, 6, lwThin, al, &c);
        break;
      }
      case 1: {  // sparkle
        for (int k = 0; k < 4; k++) {
          float an = (float)k * TAU * 0.25f;
          Vec2 l[2] = { { ax + fcos(an) * a * 0.35f, ay + fsin(an) * a * 0.35f },
                        { ax + fcos(an) * a, ay + fsin(an) * a } };
          I.sline(l, 2, lwThin, al, &c);
        }
        break;
      }
      case 2: case 3: {  // heart (3 = broken)
        Vec2 h[7] = { { ax, ay + a * 0.9f }, { ax - a, ay - a * 0.1f }, { ax - a * 0.55f, ay - a * 0.8f },
                      { ax, ay - a * 0.25f }, { ax + a * 0.55f, ay - a * 0.8f }, { ax + a, ay - a * 0.1f },
                      { ax, ay + a * 0.9f } };
        P96 sm; chaikin(sm, h, 7, false, 1);
        I.sline(sm.p, sm.n, lwThin * 1.1f, al, &c);
        if (f.aura == 3) {
          Vec2 z1[4] = { { ax, ay - a * 0.35f }, { ax - a * 0.18f, ay }, { ax + a * 0.12f, ay + a * 0.3f }, { ax - a * 0.08f, ay + a * 0.7f } };
          I.sline(z1, 4, lwThin, al, &c);
        }
        break;
      }
      case 4: {  // flower
        for (int k = 0; k < 5; k++) {
          float an = (float)k / 5 * TAU;
          I.dot(ax + fcos(an) * a * 0.65f, ay + fsin(an) * a * 0.65f, a * 0.33f, c, al);
        }
        I.dot(ax, ay, a * 0.28f, I.ink, 0.75f);
        break;
      }
      case 5: {  // peace
        P96 e; ellipsePts(e, ax, ay, a, a, 14);
        Vec2 tmp[17]; for (int i2 = 0; i2 < e.n; i2++) tmp[i2] = e.p[i2];
        tmp[e.n] = e.p[0];
        I.sline(tmp, e.n + 1, lwThin, al, &c);
        Vec2 l1[2] = { { ax, ay - a }, { ax, ay + a } };
        I.sline(l1, 2, lwThin, al, &c);
        for (int sd = -1; sd <= 1; sd += 2) {
          Vec2 l2[2] = { { ax, ay }, { ax + sd * a * 0.7f, ay + a * 0.7f } };
          I.sline(l2, 2, lwThin, al, &c);
        }
        break;
      }
      case 6: {  // music note
        Vec2 stem[2] = { { ax + a * 0.5f, ay - a }, { ax + a * 0.5f, ay + a * 0.55f } };
        I.sline(stem, 2, lwThin, al, &c);
        I.dot(ax + a * 0.28f, ay + a * 0.62f, a * 0.3f, c, al + 0.1f);
        Vec2 flag[3] = { { ax + a * 0.5f, ay - a }, { ax + a * 0.95f, ay - a * 0.6f }, { ax + a * 0.8f, ay - a * 0.15f } };
        I.sline(flag, 3, lwThin, al, &c);
        break;
      }
      case 7: {  // bolt
        Vec2 b[4] = { { ax - a * 0.2f, ay - a }, { ax + a * 0.25f, ay - a * 0.15f }, { ax - a * 0.1f, ay - a * 0.05f }, { ax + a * 0.3f, ay + a } };
        I.sline(b, 4, lwThin * 1.3f, al + 0.1f, &c);
        break;
      }
      default: {  // sun
        P96 e; ellipsePts(e, ax, ay, a * 0.55f, a * 0.55f, 12);
        Vec2 tmp[15]; for (int i2 = 0; i2 < e.n; i2++) tmp[i2] = e.p[i2];
        tmp[e.n] = e.p[0];
        I.sline(tmp, e.n + 1, lwThin, al, &c);
        for (int k = 0; k < 8; k++) {
          float an = (float)k / 8 * TAU;
          Vec2 l[2] = { { ax + fcos(an) * a * 0.75f, ay + fsin(an) * a * 0.75f },
                        { ax + fcos(an) * a * 1.1f, ay + fsin(an) * a * 1.1f } };
          I.sline(l, 2, lwThin, al, &c);
        }
        break;
      }
    }
  }

  // ------------------------------------------------ the whole portrait
  void paint(float cx, float cy, float scale) {
    s = scale * f.sizeJ;
    I.setXform(cx, cy, f.roll);
    lwMain = s * 0.045f * f.press;
    lwThin = s * 0.021f * f.press;
    if (lwMain < 1.4f) lwMain = 1.4f;
    if (lwThin < 0.8f) lwThin = 0.8f;
    I.ink = INKS[f.idx[C_INK]].c;
    baseInk = I.ink;

    buildHead();
    computeEyes();      // brows/fringe/quirks all need eye positions
    bgDeco();
    hairBehind();
    headBase();
    scaffold();
    headContour();
    shading();
    ears();
    hairAndHat();
    brows();
    eyes();
    nose();
    eyewear();
    facialHair();
    mouth();
    quirks();
    garmentAndNeck();
    aura();
    I.clearClip();
  }
};

// Paper background with speckle. Call once per frame before faces.
static inline void paperBackground(Canvas& cv, uint32_t seed) {
  int W = cv.width(), H = cv.height();
  uint8_t base[3] = { PAPER_C.r, PAPER_C.g, PAPER_C.b };
  for (int y = 0; y < H; y++)
    for (int x = 0; x < W; x++) {
      uint32_t h = (uint32_t)(x * 73856093u) ^ (uint32_t)(y * 19349663u) ^ seed;
      h = (h ^ (h >> 13)) * 0x5bd1e995u;
      float n = (float)((h >> 8) & 0xFF) / 255.0f - 0.5f;
      uint8_t c[3];
      for (int i = 0; i < 3; i++) {
        int v = base[i] + (int)(n * 7);
        c[i] = (uint8_t)(v < 0 ? 0 : v > 255 ? 255 : v);
      }
      cv.blend(x, y, c, 1.0f);
    }
  Rng R(seed ^ 0xABCDu);
  int specks = W * H / 900;
  uint8_t dark[3] = { 150, 143, 132 };
  for (int i = 0; i < specks; i++) {
    int x = (int)R.rr(0, (float)W), y = (int)R.rr(0, (float)H);
    cv.blend(x, y, dark, R.rr(0.06f, 0.22f));
  }
}

// Convenience entry point.
// covBuf must be width*height bytes of scratch (the stroke coverage buffer).
static inline void drawFace(Canvas& cv, uint8_t* covBuf, const FaceTraits& f,
                            float cx, float cy, float scale, uint32_t strokeSeed = 0,
                            int speed = 0) {
  Ink ink;
  Rng strokeR(strokeSeed ^ f.seed ^ 0x51ED270Bu);
  ink.begin(&cv, &strokeR, covBuf);
  ink.paper = PAPER_C;
  ink.speed = speed;
  if (scale < 55) { ink.boost = 1.22f; ink.minW = 0.9f; }
  FacePainter fp(ink, f, strokeSeed);
  fp.R = strokeR;
  fp.paint(cx, cy, scale);
}

} // namespace dd
