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

typedef struct sDialogueBase {
  Character character_left;
  Character character_right;
  CharacterSide character_speaking;
  const char* line;
  u next_line;
} DialogueBase;

static void dia_init();
static const char* get_char_name(Character c);



#ifdef      DIALOGUES_IMPLEMENTATION
// dialogues implementation



typedef struct sDialogueInitState {
  u line_num;
  Character left_char;
  Character right_char;
} DialogueInitState;

#define MAX_DIALOGUES 100

static DialogueBase dia_main[MAX_DIALOGUES] = {0};

static Texture char_app[10] = {0};


static void set_line(DialogueInitState* dis, u line_num, CharacterSide speaking_char, const char* line, u next_line_num);
static void say(DialogueInitState* dis, CharacterSide speaking_char, const char* line);
static void lsay(DialogueInitState* dis, const char* nuts);
static void rsay(DialogueInitState* dis, const char* nuts);
static void go_to(DialogueInitState* dis, u to);
static void repeat_last(DialogueInitState* dis);


static void set_line(DialogueInitState* dis, u line_num, CharacterSide speaking_char, const char* line, u next_line_num) {
  dia_main[line_num] = (DialogueBase){dis->left_char, dis->right_char, speaking_char, line, next_line_num};
}

static void say(DialogueInitState* dis, CharacterSide speaking_char, const char* line) {
  set_line(dis, dis->line_num, speaking_char, line, dis->line_num+1);
  dis->line_num++;
}
static void lsay(DialogueInitState* dis, const char* nuts) {
  say(dis, C_LEFT, nuts);
}
static void rsay(DialogueInitState* dis, const char* nuts) {
  say(dis, C_RIGHT, nuts);
}
static void go_to(DialogueInitState* dis, u to) {
  set_line(dis, dis->line_num, C_NULL, NULL, to);
}
static void repeat_last(DialogueInitState* dis) {
  go_to(dis, dis->line_num-1);
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
    default: return "IDK";
  }
}

static void dia_init() {

  char_app[CHAR_NULL]        = load_texture("resources/sprites/hexes/null.png" );
  char_app[CHAR_MERSEUS]     = load_texture("resources/sprites/characters/merseus.png" );
  char_app[CHAR_FARADEUS]    = load_texture("resources/sprites/characters/faradeus.png" );
  char_app[CHAR_IMPUS]       = load_texture("resources/sprites/characters/impus.png" );
  char_app[CHAR_VAMPIRUS]    = load_texture("resources/sprites/characters/vampirus.png" );
  char_app[CHAR_SHADOWUS]    = load_texture("resources/sprites/characters/shadowus.png" );
  char_app[CHAR_CERBERUS_JR] = load_texture("resources/sprites/characters/cerberus_jr.png" );
  char_app[CHAR_ANGELUS]     = load_texture("resources/sprites/characters/angelus.png" );
  char_app[CHAR_PIDGEONUS]   = load_texture("resources/sprites/characters/pidgeonus.png" );
  char_app[CHAR_BLOODLETTUS] = load_texture("resources/sprites/characters/bloodlettus.png" );


  DialogueInitState  dis0 =   {0};
  DialogueInitState* dis  = &dis0;
  dis->left_char = CHAR_FARADEUS;
  dis->right_char = CHAR_MERSEUS;

  lsay(dis, "sup");
  rsay(dis, "sup to you too");

#define test_char_say(CHARACTER) do { \
  dis->right_char = CHAR_##CHARACTER; rsay(dis, "I am " #CHARACTER); lsay(dis, "Hi, " #CHARACTER);\
} while(0)

test_char_say(MERSEUS);
test_char_say(FARADEUS);
test_char_say(IMPUS);
test_char_say(VAMPIRUS);
test_char_say(SHADOWUS);
test_char_say(CERBERUS_JR);
test_char_say(ANGELUS);
test_char_say(PIDGEONUS);
test_char_say(BLOODLETTUS);

  repeat_last(dis);



}





#endif   // DIALOGUES_IMPLEMENTATION
#endif   //_DIALOGUES_H_

