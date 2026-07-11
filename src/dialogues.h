// #define DIALOGUES_IMPLEMENTATION
#ifndef    _DIALOGUES_H_
#define    _DIALOGUES_H_
// dialogues interface


typedef u DialogueLine;
typedef enum {
CHAR_NULL = 0,
CHAR_MERSEUS,
CHAR_FARADEUS,
CHAR_IMPUS,
CHAR_VAMPIRUS,
CHAR_SHADOWUS,
CHAR_CERBERUS_JR,
CHAR_ANGELUS,
CHAR_PIDGEONUS,
CHAR_BLOODLETTUS,
} Character;

typedef enum {
  C_NULL = 0,
  C_LEFT,
  C_RIGHT,
} CharacterSide;

typedef enum {
  SONG_NULL = 0,
  SONG_SHRINE,
  SONG_DEEP,
  SONG_HIMA,
  SONG_FOREST,
} Song;

typedef struct sDialogueBase {
  Character character_left;
  Character character_right;
  CharacterSide character_speaking;
  const char* line;
  u next_line;
  HexTrait wait_for;
  u after_wait_line;
  Song song;
  bool roll_credits;
} DialogueBase;


static void dia_init();
static void dia_deinit();
static const char* get_char_name(Character c);



#ifdef      DIALOGUES_IMPLEMENTATION
// dialogues implementation



typedef struct sDialogueInitState {
  u line_num;
  Character left_char;
  Character right_char;
  u wait_line;
} DialogueInitState;

#define MAX_DIALOGUES 250

static DialogueBase dia_main[MAX_DIALOGUES] = {0};

static Texture char_app[10] = {0};
static Texture char_hand[10] = {0};


static void set_line(DialogueInitState* dis, u line_num, CharacterSide speaking_char, const char* line, u next_line_num, HexTrait wait_for, u after_wait_line, Song play_song, bool roll_credits) ;
static void say(DialogueInitState* dis, CharacterSide speaking_char, const char* line);
static void lsay(DialogueInitState* dis, const char* nuts);
static void rsay(DialogueInitState* dis, const char* nuts);
static void go_to(DialogueInitState* dis, u to);
static void repeat_last(DialogueInitState* dis);
static void stop(DialogueInitState* dis);
static void reveal_hex(DialogueInitState* dis, HexTrait hex);


static void set_line(DialogueInitState* dis, u line_num, CharacterSide speaking_char, const char* line, u next_line_num, HexTrait wait_for, u after_wait_line, Song play_song, bool roll_credits) {
  dia_main[line_num] = (DialogueBase){dis->left_char, dis->right_char, speaking_char, line, next_line_num, wait_for, after_wait_line, play_song, roll_credits};
  dis->line_num++;
}

static void say(DialogueInitState* dis, CharacterSide speaking_char, const char* line) {
  set_line(dis, dis->line_num, speaking_char, line, dis->line_num+1, HEX_NULL, 0, SONG_NULL, false);
}
static void lsay(DialogueInitState* dis, const char* nuts) {
  say(dis, C_LEFT, nuts);
}
static void rsay(DialogueInitState* dis, const char* nuts) {
  say(dis, C_RIGHT, nuts);
}
static void rloop(DialogueInitState* dis, const char* line) {
  stop(dis);
  rsay(dis, line);
  repeat_last(dis);
}

static void go_to(DialogueInitState* dis, u to) {
  set_line(dis, dis->line_num, C_NULL, NULL, to, HEX_NULL, 0, SONG_NULL, false);
}
static void repeat_last(DialogueInitState* dis) {
  go_to(dis, dis->line_num-1);
}

static void stop(DialogueInitState* dis) {
  set_line(dis, dis->line_num, C_NULL, NULL, dis->line_num+1, HEX_NULL, 0, SONG_NULL, false);
}
static void to_song(DialogueInitState* dis, Song song) {
  set_line(dis, dis->line_num, C_NULL, NULL, dis->line_num+1, HEX_NULL, 0, song, false);
}
static const char* get_char_name(Character c) {
  switch (c) {
case CHAR_MERSEUS:     return "Merseus";
case CHAR_FARADEUS:    return "Faradeus";
case CHAR_IMPUS:       return "Impus";
case CHAR_VAMPIRUS:    return "Vampirus";
case CHAR_SHADOWUS:    return "Shadowus";
case CHAR_CERBERUS_JR: return "Cerberus junior";
case CHAR_ANGELUS:     return "Angelus";
case CHAR_PIDGEONUS:   return "Pidgeonus";
case CHAR_BLOODLETTUS: return "Bloodlettus";
    default: return "";
  }
}
static void wait_for(DialogueInitState* dis, HexTrait hex) {
  dis->wait_line = dis->line_num;
  set_line(dis, dis->line_num, C_NULL, NULL, dis->line_num+1, hex, 0, SONG_NULL, false);
}
static void reveal_hex(DialogueInitState* dis, HexTrait hex) {
  set_line(dis, dis->line_num, C_NULL, NULL, dis->line_num+1, hex, 0, SONG_NULL, false);
}

static void wait_end(DialogueInitState* dis) {
  dia_main[dis->wait_line].after_wait_line = dis->line_num;
}
static void roll_credits(DialogueInitState* dis) {
  set_line(dis, dis->line_num, C_NULL, NULL, dis->line_num+1, HEX_NULL, 0, SONG_NULL, true);
}


static void dia_init() {

  char_app[CHAR_NULL]        = (Texture2D){ 0 };
  char_app[CHAR_MERSEUS]     = load_texture("resources/sprites/characters/merseus.png" );
  char_app[CHAR_FARADEUS]    = load_texture("resources/sprites/characters/faradeus.png" );
  char_app[CHAR_IMPUS]       = load_texture("resources/sprites/characters/impus.png" );
  char_app[CHAR_VAMPIRUS]    = load_texture("resources/sprites/characters/vampirus.png" );
  char_app[CHAR_SHADOWUS]    = load_texture("resources/sprites/characters/shadowus.png" );
  char_app[CHAR_CERBERUS_JR] = load_texture("resources/sprites/characters/cerberus_jr.png" );
  char_app[CHAR_ANGELUS]     = load_texture("resources/sprites/characters/angelus.png" );
  char_app[CHAR_PIDGEONUS]   = load_texture("resources/sprites/characters/pidgeonus.png" );
  char_app[CHAR_BLOODLETTUS] = load_texture("resources/sprites/characters/bloodlettus.png" );

  char_hand[CHAR_NULL]        = (Texture2D){ 0 };
  char_hand[CHAR_MERSEUS]     = load_texture("resources/sprites/characters/hand_of_merseus.png" );
  char_hand[CHAR_FARADEUS]    = load_texture("resources/sprites/hexes/null.png");
  char_hand[CHAR_IMPUS]       = load_texture("resources/sprites/characters/hand_of_impus.png" );
  char_hand[CHAR_VAMPIRUS]    = load_texture("resources/sprites/characters/hand_of_vampirus.png" );
  char_hand[CHAR_SHADOWUS]    = load_texture("resources/sprites/characters/hand_of_shadowus.png" );
  char_hand[CHAR_CERBERUS_JR] = load_texture("resources/sprites/characters/hand_of_cerberus_jr.png" );
  char_hand[CHAR_ANGELUS]     = load_texture("resources/sprites/characters/the_tournment_nexus.png" );
  char_hand[CHAR_PIDGEONUS]   = load_texture("resources/sprites/characters/hand_of_pidgeonus.png" );
  char_hand[CHAR_BLOODLETTUS] = load_texture("resources/sprites/characters/hand_of_bloodlettus.png" );


  DialogueInitState  dis0 =   {0};
  DialogueInitState* dis  = &dis0;
  dis->left_char = CHAR_FARADEUS;
#include "dialogue.c"
//  printf("+========= %zu ==========+", dis->line_num);

}
static void dia_deinit() {
  unload_texture(char_app[CHAR_MERSEUS]    );
  unload_texture(char_app[CHAR_FARADEUS]   );
  unload_texture(char_app[CHAR_IMPUS]      );
  unload_texture(char_app[CHAR_VAMPIRUS]   );
  unload_texture(char_app[CHAR_SHADOWUS]   );
  unload_texture(char_app[CHAR_CERBERUS_JR]);
  unload_texture(char_app[CHAR_ANGELUS]    );
  unload_texture(char_app[CHAR_PIDGEONUS]  );
  unload_texture(char_app[CHAR_BLOODLETTUS]);

  unload_texture(char_hand[CHAR_MERSEUS]    );
  unload_texture(char_hand[CHAR_FARADEUS]   );
  unload_texture(char_hand[CHAR_IMPUS]      );
  unload_texture(char_hand[CHAR_VAMPIRUS]   );
  unload_texture(char_hand[CHAR_SHADOWUS]   );
  unload_texture(char_hand[CHAR_CERBERUS_JR]);
  unload_texture(char_hand[CHAR_ANGELUS]    );
  unload_texture(char_hand[CHAR_PIDGEONUS]  );
  unload_texture(char_hand[CHAR_BLOODLETTUS]);
}





#endif   // DIALOGUES_IMPLEMENTATION
#endif   //_DIALOGUES_H_

