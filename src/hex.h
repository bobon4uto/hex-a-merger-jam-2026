// #define HEX_IMPLEMENTATION
#ifndef    _HEX_H_
#define    _HEX_H_
// hex interface

#include "no.util/core.h"


#define NUMBER_OF_HEXES 21
typedef enum {
  HEX_NULL = 0,
  HEX_REVERSO,
  // sensory gang (death path)
  HEX_BLIND,
  HEX_DEAF,
  HEX_ANOSMIA,
  HEX_SOMATOSENSORY_LOSS,
  HEX_AGEUSIA,

  HEX_DEATH,

  // inactive gang (famine path)
  HEX_WEAK,
  HEX_SILENCE,
  HEX_PROCRASTINATION,

  HEX_FAMINE,

  // unwell gang (plague path)
  HEX_FLU,
  HEX_INSANITY,
  HEX_CANCER,
  HEX_AMNESIA,

  HEX_PLAGUE,

  // violence gang (war path)
  HEX_AGGRESION,
  HEX_VIOLENCE,
  HEX_DISCRIMINATION,

  HEX_WAR,
} HexTrait;
NOU(HexTrait);

typedef struct sHex {
  bool complete;
  DA_(HexTrait) traits;
} Hex;
static void hex_merge(Hex* self, Hex* other);
static void hex_finalize(Hex* self);
static void try_merge(Hex* self, HexTrait into);
static void hex_reverse(Hex* self);
static HexTrait hex_reverso(HexTrait t);


#ifdef      HEX_IMPLEMENTATION
// hex implementation

HexTrait* merge_crafts[NUMBER_OF_HEXES] = {0};
HexTrait null_merge[] = {
  HEX_NULL };
HexTrait reverso_merge[] = { HEX_REVERSO,
  HEX_NULL };
HexTrait blind_merge[] = { HEX_SOMATOSENSORY_LOSS, HEX_REVERSO,
  HEX_NULL };
HexTrait deaf_merge[] = { HEX_BLIND, HEX_WEAK,
  HEX_NULL };
HexTrait anosmia_merge[] = { HEX_FLU, HEX_WEAK, HEX_NULL };
HexTrait somatosensory_loss_merge[] = { HEX_BLIND, HEX_REVERSO,
  HEX_NULL };
HexTrait ageusia_merge[] = { HEX_VIOLENCE, HEX_DEAF,
  HEX_NULL };
HexTrait death_merge[] = {
  HEX_BLIND,
  HEX_DEAF,
  HEX_ANOSMIA,
  HEX_SOMATOSENSORY_LOSS,
  HEX_AGEUSIA,
    HEX_NULL
  };

HexTrait weak_merge[] = { HEX_AGGRESION, HEX_REVERSO,
  HEX_NULL };
HexTrait silence_merge[] = { HEX_DEAF, HEX_FLU,
  HEX_NULL };
HexTrait procrastination_merge[] = { HEX_INSANITY, HEX_SILENCE,
  HEX_NULL};
HexTrait famine_merge[] = {
  HEX_WEAK,
  HEX_SILENCE,
  HEX_PROCRASTINATION,
    HEX_NULL
  };

HexTrait flu_merge[] = { HEX_FLU,
  HEX_NULL };
HexTrait insanity_merge[] = {
  HEX_FLU,
  HEX_AGGRESION,
    HEX_NULL
  };
HexTrait cancer_merge[] = { HEX_FLU, HEX_VIOLENCE, HEX_ANOSMIA, HEX_PROCRASTINATION,
    HEX_NULL
};
HexTrait amnesia_merge[] = { HEX_AGEUSIA, HEX_BLIND, HEX_SILENCE,
  HEX_NULL };
HexTrait plague_merge[] = {
  HEX_FLU,
  HEX_INSANITY,
  HEX_CANCER,
  HEX_AMNESIA,
    HEX_NULL
  };

HexTrait aggresion_merge[] = { HEX_WEAK, HEX_REVERSO,
  HEX_NULL };
HexTrait violence_merge[] = { HEX_BLIND, HEX_AGGRESION,
  HEX_NULL };
HexTrait discrimination_merge[] = { HEX_SILENCE, HEX_REVERSO,
  HEX_NULL };
HexTrait war_merge[] = {
  HEX_AGGRESION,
  HEX_VIOLENCE,
  HEX_DISCRIMINATION,
    HEX_NULL
  };
static HexTrait hex_reverso(HexTrait t) {
#define REVERSO(H1, H2) case H1: return H2; case H2: return H1
  switch (t) {
REVERSO(HEX_WEAK,HEX_AGGRESION);
REVERSO(HEX_DISCRIMINATION,HEX_SILENCE);
REVERSO(HEX_BLIND,HEX_SOMATOSENSORY_LOSS);


  case HEX_WAR: return HEX_AGGRESION;
  case HEX_FAMINE: return HEX_WEAK;
  case HEX_PLAGUE: return HEX_FLU;
  case HEX_DEATH: return HEX_DEATH; // maybe put some sound effect in that exact moment?
    default: return t;
  }
}




static void hex_merge(Hex* self, Hex* other) {
  bool reverse = false;
  foreach(HexTrait as trait in other->traits) {
    if (trait == HEX_REVERSO) {
      reverse = true;
      continue;
    }
    da_push(&self->traits, trait);
  }
  da_free(&other->traits);
  if (reverse) {
    hex_reverse(self);
  }
  hex_finalize(self);
}
static void hex_reverse(Hex* self) {
  foreach(HexTrait in self->traits) {
    *current=hex_reverso(item);
  }
}


static void hex_finalize(Hex* self) {

try_merge(self, HEX_DEATH);
try_merge(self, HEX_FAMINE);
try_merge(self, HEX_PLAGUE);
try_merge(self, HEX_WAR);

try_merge(self, HEX_AGEUSIA);
try_merge(self, HEX_CANCER);
try_merge(self, HEX_AMNESIA);
try_merge(self, HEX_PROCRASTINATION);

try_merge(self, HEX_ANOSMIA);
try_merge(self, HEX_SILENCE);
try_merge(self, HEX_INSANITY);
try_merge(self, HEX_VIOLENCE);
try_merge(self, HEX_DEAF);

}


static void try_merge(Hex* self, HexTrait into) {
  HexTrait* ingridients = merge_crafts[into];
  for (; *ingridients!=HEX_NULL; ++ingridients) {
    bool found = false;
    foreach (HexTrait as trait in self->traits) {
      if (trait == *ingridients) {
        found = true;
        break;
      }
    }
    if (!found) {
      return;
    }
  }
  // found all ingridiens, lets craft.

  self->traits.count = 0;
  da_push(&self->traits, into);
}



#endif   // HEX_IMPLEMENTATION
#endif   //_HEX_H_

