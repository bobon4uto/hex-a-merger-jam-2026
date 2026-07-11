/*******************************************************************************************
*
*   Hex a merger
*
*   Code licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2026 Vladimir Petrenko (@bobon4uto)
*
********************************************************************************************/

#include "snake_case_api.h"
#include "raymath_snake_case.h"
#include "raylib.h"
#include "raymath.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>      // Emscripten library
#endif

#include <stdio.h>                          // Required for: printf()
#include <stdlib.h>                         // Required for: 
#include <string.h>                         // Required for:

  #define NOU_STRING_BUILDER_IMPLEMENTATION
  #define NOU_STRING_VIEW_IMPLEMENTATION
// this is basically my utils
#include "no.util/core.h"

  #define HEX_IMPLEMENTATION
#include "hex.h"


#define RESOURCES_IMPLEMENTATION
#include "resources.h"


#define DIALOGUES_IMPLEMENTATION
#include "dialogues.h"
//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
// Simple log system to avoid printf() calls if required
// NOTE: Avoiding those calls, also avoids const strings memory usage
#define SUPPORT_LOG_INFO
#if defined(SUPPORT_LOG_INFO)
    #define LOG(...) printf(__VA_ARGS__)
    #define LOG_ERR(...) fprintf(stderr, __VA_ARGS__)
#else
    #define LOG(...)
    #define LOG_ERR(...)
#endif
#define WEAK_BLUE       CLITERAL(Color){0x55,0x55,0xaa,0xff}     // Weak Blue
#define PAPER_WHITE     CLITERAL(Color){0xDF,0xAF,0x5F,0xFF}     // Weak Blue



#define GENERAL_PANIC(PANIC_REASON, ...) do {LOG_ERR(PANIC_REASON": "); LOG_ERR(__VA_ARGS__); exit(0); } while (0)
#define UNREACHABLE(...) GENERAL_PANIC("UNREACHABLE", __VA_ARGS__)

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum {
  SCREEN_LOGO = 0,
  SCREEN_TITLE,
  SCREEN_GAMEPLAY,
  SCREEN_STORY,
  SCREEN_BOOK,
  SCREEN_BOOK_PAGE2,
  SCREEN_SETTINGS,
  SCREEN_ENDING
} GameScreen;


typedef struct sSettings{
  float music_volume;
} Settings;

typedef struct sInput{
  Vector2 pointer_position;
  bool        pressed, down, released;
} Input;

typedef struct sHexagonButton {
  GameScreen active_on;
  Vector2 position;
  Vector2 target_position;
  Vector2 home_position;
  float radius;
  float thickness;
  float rotation;
  Color color_inside;
  Color color_outside;
  bool pressed, down, released, not_active, was_in_range, fixed, other;
  float rotation_speed;
  bool has_hex;
  Hex hex;
} HexagonButton;
NOU(HexagonButton);

typedef struct sMusicBanner {
  float animation_timer;
  MetaMusic music;
} MusicBanner;


typedef struct sStringBuilderWrapper {
  StringBuilder sb;
  Rectangle screen_dim;
  const char* link;
  bool is_focused, is_down;
} StringBuilderWrapper ;


#define G_STEP_X_X (-90.0f)
#define G_STEP_X_Y 52.0f
#define G_STEP_Y_X 90.0f
#define G_STEP_Y_Y 52.0f
// 450 412
//90 52
#define G_BASE_X 360.0f
#define G_BASE_Y 60.0f
#define G_(X,Y) (Vector2){G_BASE_X + G_STEP_X_X*X + G_STEP_Y_X*Y, G_BASE_Y  + G_STEP_X_Y*X + G_STEP_Y_Y*Y}
// macro slop
  //                          bool pressed, down, released;
// :btn

#define PLAYABLE_BTN(T, X,Y) T(gameplay_grid_##X##_##Y, \
      { return button_init_playable(G_(X,Y)); }, \
      {},{ button.gameplay_grid_##X##_##Y.color_inside = fade(RED, 0.05); },{ if (button.gameplay_grid_##X##_##Y.was_in_range) {on_release_playable(X,Y);} \
      button.gameplay_grid_##X##_##Y.color_inside = fade(WHITE, 0.05f); } \
    )\

#define GRAB(N) if (grabbed_hex) {reset_grabbed();} grabbed_hex = &button.gameplay_draggable_hex_##N

#define BOOK_BASE_X 120.0f
#define BOOK_BASE_Y 180.0f
#define BOOK_STEP_X 120.0f
#define BOOK_STEP_Y 120.0f
#define BOOK_COORDS(X,Y) (Vector2){BOOK_BASE_X+BOOK_STEP_X*X, BOOK_BASE_Y+BOOK_STEP_Y*Y}

#define BOOK_BTN(T,X,Y, NAME) \
    \
    T(book_##NAME##_highlight, \
      { HexagonButton self = button_init_default(SCREEN_BOOK, BOOK_COORDS(X,Y), 70.0f); \
      self.not_active = true; \
        self.color_outside = RED; return self; }, \
      {} ,{},{} \
    ) \
    \
    T(book_##NAME##_description, \
      { return button_init_book(BOOK_COORDS(X,Y), HEX_##NAME); }, \
      { if (hex_is_shown[HEX_##NAME]) { show_recepie(HEX_##NAME); } } ,{},{} \
    ) \

#define HEX_BUTTONS_LIST(T) \
  T(gameplay_to_settings, \
      { return button_init_color(SCREEN_GAMEPLAY, (Vector2){660.0f,60.0f}, 60.0f, fade(PAPER_WHITE, 0.2),BLACK); }, \
      { previous_screen = current_screen; current_screen = SCREEN_SETTINGS; },{},{} \
    ) \
  T(gameplay_to_story, \
      { return button_init_color(SCREEN_GAMEPLAY, (Vector2){60.0f,60.0f}, 60.0f, fade(PAPER_WHITE, 0.1),fade(PAPER_WHITE, 0.2) ); }, \
      { current_screen = SCREEN_STORY; next_line(); },{},{\
      if (button.gameplay_to_story.was_in_range) { \
          if (grabbed_hex) { \
            if (grabbed_hex->hex.traits.count == 1) { \
              if (da_first(&grabbed_hex->hex.traits) == waiting_for) { \
                waiting_for = HEX_NULL; \
                if (grabbed_hex->other) { \
                          u position = (grabbed_hex - draggable_buttons_on_grid.items); \
                          da_remove_at(&draggable_buttons_on_grid, position); \
                          grabbed_hex = NULL; \
                } else { \
                  grabbed_hex->position = vector2_add( grabbed_hex->home_position, (Vector2){0.0f,240.0f} ); \
                  da_free(&grabbed_hex->hex.traits); \
                  new_hex(grabbed_hex); \
                } \
                current_dialogue_line = after_wait_line; \
                current_screen = SCREEN_STORY; \
                next_line(); \
              } else { \
                current_screen = SCREEN_STORY; \
                next_line(); \
              } \
            } else { \
              current_screen = SCREEN_STORY; \
              next_line(); \
            } \
          } \
        } \
      }\
    ) \
  \
  T(story_next, \
      { HexagonButton self = button_init_default(SCREEN_STORY, (Vector2){640.0f,640.0f}, 80.0f); \
      self.rotation_speed = 0.5f; \
      return self; }, \
      { next_line(); },{},{} \
    ) \
  T(gameplay_to_book, \
      { return button_init_color(SCREEN_GAMEPLAY, (Vector2){70.0f,480.0f}, 70.0f, fade(PAPER_WHITE, 0.1),fade(PAPER_WHITE, 0.2)); }, \
      { current_screen = SCREEN_BOOK; },{},{ \
        if (button.gameplay_to_book.was_in_range) { \
          if(grabbed_hex) { \
            show_contents(grabbed_hex->hex); current_screen = SCREEN_BOOK; \
          } \
        } \
      } \
    ) \
  T(book_to_gameplay, \
      { return button_init_color(SCREEN_BOOK, (Vector2){660.0f,60.0f}, 60.0f, RED, BLACK); }, \
      { current_screen = SCREEN_GAMEPLAY; },{},{} \
    ) \
  \
  BOOK_BTN(T,0,0,AGGRESION) \
  BOOK_BTN(T,0,1,VIOLENCE) \
  BOOK_BTN(T,0,2,DISCRIMINATION) \
  \
  BOOK_BTN(T,0,3,WAR) \
  \
  BOOK_BTN(T,1,0,BLIND) \
  BOOK_BTN(T,1,1,DEAF) \
  BOOK_BTN(T,1,2,ANOSMIA) \
  BOOK_BTN(T,2,0,SOMATOSENSORY_LOSS) \
  BOOK_BTN(T,2,1,AGEUSIA) \
  \
  BOOK_BTN(T,1,3,DEATH) \
  \
  BOOK_BTN(T,3,0,FLU) \
  BOOK_BTN(T,3,1,INSANITY) \
  BOOK_BTN(T,3,2,AMNESIA) \
  BOOK_BTN(T,2,2,CANCER) \
  \
  BOOK_BTN(T,3,3,PLAGUE) \
  \
  BOOK_BTN(T,4,0,WEAK) \
  BOOK_BTN(T,4,1,SILENCE) \
  BOOK_BTN(T,4,2,PROCRASTINATION) \
  \
  BOOK_BTN(T,4,3,FAMINE) \
  \
  BOOK_BTN(T,2,3,REVERSO) \
  \
  \
  \
  PLAYABLE_BTN(T,0,0) \
  PLAYABLE_BTN(T,0,1) \
  PLAYABLE_BTN(T,0,2) \
  \
  \
  PLAYABLE_BTN(T,1,0) \
  PLAYABLE_BTN(T,1,1) \
  PLAYABLE_BTN(T,1,2) \
  PLAYABLE_BTN(T,1,3) \
  \
  PLAYABLE_BTN(T,2,0) \
  PLAYABLE_BTN(T,2,1) \
  PLAYABLE_BTN(T,2,2) \
  PLAYABLE_BTN(T,2,3) \
  PLAYABLE_BTN(T,2,4) \
  \
  PLAYABLE_BTN(T,3,1) \
  PLAYABLE_BTN(T,3,2) \
  PLAYABLE_BTN(T,3,3) \
  PLAYABLE_BTN(T,3,4) \
  \
  \
  PLAYABLE_BTN(T,4,2) \
  PLAYABLE_BTN(T,4,3) \
  PLAYABLE_BTN(T,4,4) \
  T(gameplay_draggable_hex_1, \
      { return button_init_hex((Vector2){120.0f,600.0f}, HEX_AGGRESION); }, \
      { GRAB(1); },{},{ /*release is global*/ } \
    ) \
  T(gameplay_draggable_hex_2, \
      { return button_init_hex((Vector2){240.0f,600.0f}, HEX_BLIND); }, \
      { GRAB(2); },{},{ } \
    ) \
  T(gameplay_draggable_hex_3, \
      { return button_init_hex((Vector2){360.0f,600.0f}, HEX_REVERSO); }, \
      { GRAB(3); },{},{ } \
    ) \
  T(gameplay_draggable_hex_4, \
      { return button_init_hex((Vector2){480.0f,600.0f}, HEX_FLU); }, \
      { GRAB(4); },{},{ } \
    ) \
  T(gameplay_draggable_hex_5, \
      { return button_init_hex((Vector2){600.0f,600.0f}, HEX_WEAK); }, \
      { GRAB(5); },{},{ } \
    ) \
  T(settings_back_to_previous, \
      { return button_init_color(SCREEN_SETTINGS, (Vector2){660.0f,60.0f}, 60.0f, RED, BLACK); }, \
      { current_screen = previous_screen; },{},{} \
    ) \
  T(settings_music_volume, \
      { return button_init_color(SCREEN_SETTINGS, (Vector2){500.0f,500.0f}, 150.0f, fade(WHITE,0.0f), BLACK); }, \
      { button.settings_music_volume.other = true; },{  \
      if (!button.settings_music_volume.other) {return;}\
      settings.music_volume = \
      fminf( button.settings_music_volume.radius, vector2_distance(button.settings_music_volume.position, input.pointer_position ) ) \
      / button.settings_music_volume.radius; \
      if (settings.music_volume < 0.2f) { settings.music_volume = 0.0f; }  \
      if (settings.music_volume > 0.8f) { settings.music_volume = 1.0f; }  \
      button.settings_music_volume_overlay.radius =  button.settings_music_volume.radius *  settings.music_volume; \
      },{ button.settings_music_volume.other = false;} \
    ) \
  T(settings_music_volume_overlay, \
      { HexagonButton  self = button_init_default(SCREEN_SETTINGS, (Vector2){500.0f,500.0f}, 150.0f);\
      self.color_inside   = WEAK_BLUE ; \
      self.color_outside  = WEAK_BLUE ; \
      self.rotation_speed = 2.0f; \
      return self; }, \
      { },{ },{} \
    ) \
  T(options, \
      { return button_init_color(SCREEN_TITLE, (Vector2){450.0f,332.0f}, 175.0f, fade(PAPER_WHITE, 0.5f), fade(BLACK, 1.0f)); }, \
      { previous_screen = current_screen; current_screen = SCREEN_SETTINGS; },{},{} \
    ) \
  T(start_game, \
      { return button_init_start_game_(); }, \
      { current_screen = SCREEN_STORY; next_line(); },{},{}\
    )

typedef struct sHexagonButtonMap {
#define UNROLL_BUTTONS_STRUCT(BUTTON_NAME,_i,_p,_d,_r) HexagonButton BUTTON_NAME;
  HEX_BUTTONS_LIST(UNROLL_BUTTONS_STRUCT)
} HexagonButtonMap;

DA_(HexagonButton) draggable_buttons_on_grid = {0};
// TODO: Define your custom data types here

//----------------------------------------------------------------------------------
// Global Variables Definition (local to this module)
//----------------------------------------------------------------------------------
static const int screen_x_ = 720;
static const int screen_y_ = 720;

static RenderTexture2D target = { 0 };  // Render texture to render our game
static int frame_counter = 0;
static GameScreen current_screen  = SCREEN_TITLE;
static GameScreen previous_screen = SCREEN_TITLE;
static Settings settings = {0};

static MusicBanner music_to_show = {0};
static Texture hex_icons[NUMBER_OF_HEXES] = {0};
static bool hex_is_known[NUMBER_OF_HEXES] = {0};
static bool hex_is_shown[NUMBER_OF_HEXES] = {0};




static HexagonButtonMap button = {0};
static HexagonButton* grabbed_hex = NULL;


static Input input = {0};

// resources
static Texture gameplay_background_texture  = { 0 };
static Texture title_background_texture     = { 0 };
static Texture settings_background_texture  = { 0 };
static Texture settings_texture             = { 0 };
static Texture lens_texture                 = { 0 };
static Texture exit_texture                 = { 0 };
static Texture ending                       = { 0 };


static Texture settings_music_icon = { 0 };
static u current_dialogue_line = 0;
//static const char* dialogue_text = "TEXT";
static DialogueBase dialogue_frame = {0};

static HexTrait waiting_for = HEX_NULL;
static u after_wait_line = 0;
static float black_amnesia_anim = -20.0f;
static StringBuilder ending_sb = {0};

static StringBuilderWrapper music_dark_shrine_loop_meta_sb = {0};
static StringBuilderWrapper music_going_deep_meta_sb = {0};
static StringBuilderWrapper music_forest_meta_sb = {0};
static StringBuilderWrapper music_unforgiving_himalayas_meta_sb = {0};
static bool cinema = false;

#ifdef _DEBUG
static Vector2 controlled_vector2 = { 0 };
static float move_speed_of_controlled_values = 0.01f;

static float controlled_float = 0.00f;
#endif // _DEBUG

// TODO: Define global variables here, recommended to make them static

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
// :funcs
static void update_and_draw_one_frame(void);      // Update and Draw one frame


static Texture texture_from_file(const char* filename);

static void update_hexagon_button(GameScreen screen, HexagonButton* self);


static void update_music_stream_ex(MetaMusic* mm);


static void show_contents(Hex hex);
static void show_recepie(HexTrait hex);

static void start_music_and_show_it(MetaMusic mm);
static void next_line();

static void on_release_playable(int x, int y);
static void reset_grabbed();
static void init_stage_1();
static bool ran_out_of_hexes();
static void new_hex(HexagonButton* button);
static HexagonButton* get_mergable( Vector2 position );
static HexTrait random_available_hex();
static void set_light(HexTrait hex, bool light);

static void hex_reveal_rec(HexTrait hex);
static void play_song(Song song);
static void update_ending();

static void draw_ending();
static void draw_raylib_logo(int x, int y);
static void draw_music_banner(int x, int y);
static void draw_texture_centered_ex(Texture texture, Vector2 position, float scale, Color tint);
static void draw_hexagon_outline(Vector2 position, float radius, float thickness, float rotation, Color color);
static void draw_hexagon_insides(Vector2 position, float radius, float rotation, Color color);

static void draw_left_character(Character c, bool is_speaking);
static void draw_right_character(Character c, bool is_speaking);

static void draw_hexagon(Vector2 position, float radius, float thickness, float rotation, Color color_inside, Color color_outside);
static void draw_hexagon_button(HexagonButton self);
static void draw_hex_trait(HexTrait hex_trait, Vector2 position,  float scale);
static void draw_title_screen();
static void draw_gameplay_screen();


static void draw_book();

static void draw_all_buttons();

#ifdef _DEBUG
static void draw_debug_overlay();
#endif // _DEBUG

static HexagonButton button_init_default(GameScreen active_on, Vector2 position, float radius);
static HexagonButton button_init_color(GameScreen active_on, Vector2 position, float radius, Color color_inside, Color color_outside);
static HexagonButton button_init_playable(Vector2 position);
#define UNROLL_BUTTONS_INIT_FUNCS(BUTTON_NAME,_i,_p,_d,_r) \
  static HexagonButton button_init_##BUTTON_NAME(); \
  static void button_##BUTTON_NAME##_on_pressed(); \
  static void button_##BUTTON_NAME##_on_down(); \
  static void button_##BUTTON_NAME##_on_released();
  HEX_BUTTONS_LIST(UNROLL_BUTTONS_INIT_FUNCS)



static HexagonButton button_init_start_game_();
static void load_music_from_meta(MetaMusic* mm);
static void pause_all_music();
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
#if !defined(_DEBUG)
    set_trace_log_level(LOG_NONE);         // Disable raylib trace log messages
#endif

    // Initialization
    //--------------------------------------------------------------------------------------
    init_window(screen_x_, screen_y_, "hex a merger");
// :init

    init_audio_device();
    load_music_from_meta(&music_dark_shrine_loop_meta);
    load_music_from_meta(&music_going_deep_meta);
    load_music_from_meta(&music_unforgiving_himalayas_meta);
    load_music_from_meta(&music_forest_meta);

    // TODO: Load resources / Initialize variables at this point
#define UNROLL_BUTTONS_INIT(BUTTON_NAME,_i,_p,_d,_r) button.BUTTON_NAME = button_init_##BUTTON_NAME();
  HEX_BUTTONS_LIST(UNROLL_BUTTONS_INIT)

    settings.music_volume = 1.0f;

    gameplay_background_texture = texture_from_file("resources/sprites/gameplay_background.png");
    title_background_texture =    texture_from_file("resources/sprites/title_back.png");
    settings_background_texture = texture_from_file("resources/sprites/settings_back.png");

    settings_texture             = texture_from_file("resources/sprites/settings.png");
    lens_texture                 = texture_from_file("resources/sprites/lens.png");
    exit_texture                 = texture_from_file("resources/sprites/exit.png");
    settings_music_icon = texture_from_file("resources/sprites/music_icon.png");

    ending = texture_from_file("resources/sprites/ending.png");


    dia_init();


    merge_crafts[HEX_NULL ] = null_merge;
    merge_crafts[HEX_REVERSO ] = reverso_merge;
    merge_crafts[HEX_BLIND ] = blind_merge;
    merge_crafts[HEX_DEAF ] = deaf_merge;
    merge_crafts[HEX_ANOSMIA ] = anosmia_merge;
    merge_crafts[HEX_SOMATOSENSORY_LOSS ] = somatosensory_loss_merge;
    merge_crafts[HEX_AGEUSIA ] = ageusia_merge;
    merge_crafts[HEX_DEATH ] = death_merge;
    merge_crafts[HEX_WEAK ] = weak_merge;
    merge_crafts[HEX_SILENCE ] = silence_merge;
    merge_crafts[HEX_PROCRASTINATION ] = procrastination_merge;
    merge_crafts[HEX_FAMINE ] = famine_merge;
    merge_crafts[HEX_FLU ] = flu_merge;
    merge_crafts[HEX_INSANITY ] = insanity_merge;
    merge_crafts[HEX_CANCER ] = cancer_merge;
    merge_crafts[HEX_AMNESIA ] = amnesia_merge;
    merge_crafts[HEX_PLAGUE ] = plague_merge;
    merge_crafts[HEX_AGGRESION ] = aggresion_merge;
    merge_crafts[HEX_VIOLENCE ] = violence_merge;
    merge_crafts[HEX_DISCRIMINATION ] = discrimination_merge;
    merge_crafts[HEX_WAR ] = war_merge;



  hex_icons[HEX_NULL] = texture_from_file("resources/sprites/hexes/null.png");
  hex_icons[HEX_REVERSO] = texture_from_file("resources/sprites/hexes/reverso.png");
  hex_icons[HEX_BLIND] = texture_from_file("resources/sprites/hexes/blind.png");
  hex_icons[HEX_DEAF] = texture_from_file("resources/sprites/hexes/deaf.png");
  hex_icons[HEX_ANOSMIA] = texture_from_file("resources/sprites/hexes/anosmia.png");
  hex_icons[HEX_SOMATOSENSORY_LOSS] = texture_from_file("resources/sprites/hexes/sematosensory_loss.png");
  hex_icons[HEX_AGEUSIA] = texture_from_file("resources/sprites/hexes/ageusia.png");
  hex_icons[HEX_DEATH] = texture_from_file("resources/sprites/hexes/death.png");
  hex_icons[HEX_WEAK] = texture_from_file("resources/sprites/hexes/weak.png");
  hex_icons[HEX_SILENCE] = texture_from_file("resources/sprites/hexes/silence.png");
  hex_icons[HEX_PROCRASTINATION] = texture_from_file("resources/sprites/hexes/procrastination.png");
  hex_icons[HEX_FAMINE] = texture_from_file("resources/sprites/hexes/famine.png");
  hex_icons[HEX_FLU] = texture_from_file("resources/sprites/hexes/flu.png");
  hex_icons[HEX_INSANITY] = texture_from_file("resources/sprites/hexes/insanity.png");
  hex_icons[HEX_CANCER] = texture_from_file("resources/sprites/hexes/cancer.png");
  hex_icons[HEX_AMNESIA] = texture_from_file("resources/sprites/hexes/amnesia.png");
  hex_icons[HEX_PLAGUE] = texture_from_file("resources/sprites/hexes/plague.png");
  hex_icons[HEX_AGGRESION] = texture_from_file("resources/sprites/hexes/aggresion.png");
  hex_icons[HEX_VIOLENCE] = texture_from_file("resources/sprites/hexes/violence.png");
  hex_icons[HEX_DISCRIMINATION] = texture_from_file("resources/sprites/hexes/discrimination.png");
  hex_icons[HEX_WAR] = texture_from_file("resources/sprites/hexes/war.png");
    //hex_is_known[HEX_NULL] = true;
    //null is the question mark
    hex_is_known[HEX_REVERSO] = true;
    hex_is_known[HEX_BLIND] = true;
    hex_is_known[HEX_WEAK] = true;
    hex_is_known[HEX_FLU] = true;
    hex_is_known[HEX_AGGRESION] = true;

    hex_is_shown[HEX_REVERSO] = true;
    hex_is_shown[HEX_BLIND] = true;
    hex_is_shown[HEX_WEAK] = true;
    hex_is_shown[HEX_FLU] = true;
    hex_is_shown[HEX_AGGRESION] = true;

    // Render texture to draw, enables screen scaling
    // NOTE: If screen is scaled, mouse input should be scaled proportionally
    target = load_render_texture(screen_x_, screen_y_);
    set_texture_filter(target.texture, TEXTURE_FILTER_BILINEAR);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(update_and_draw_one_frame, 60, 1);
#else
    set_target_f_p_s(60);     // Set our game frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!window_should_close())    // Detect window close button
    {
        update_and_draw_one_frame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    unload_texture(gameplay_background_texture);
    unload_texture(settings_music_icon);

  dia_deinit();
  unload_texture(hex_icons[HEX_NULL]);
  unload_texture(hex_icons[HEX_REVERSO]);
  unload_texture(hex_icons[HEX_BLIND]);
  unload_texture(hex_icons[HEX_DEAF]);
  unload_texture(hex_icons[HEX_ANOSMIA]);
  unload_texture(hex_icons[HEX_SOMATOSENSORY_LOSS]);
  unload_texture(hex_icons[HEX_AGEUSIA]);
  unload_texture(hex_icons[HEX_DEATH]);
  unload_texture(hex_icons[HEX_WEAK]);
  unload_texture(hex_icons[HEX_SILENCE]);
  unload_texture(hex_icons[HEX_PROCRASTINATION]);
  unload_texture(hex_icons[HEX_FAMINE]);
  unload_texture(hex_icons[HEX_FLU]);
  unload_texture(hex_icons[HEX_INSANITY]);
  unload_texture(hex_icons[HEX_CANCER]);
  unload_texture(hex_icons[HEX_AMNESIA]);
  unload_texture(hex_icons[HEX_PLAGUE]);
  unload_texture(hex_icons[HEX_AGGRESION]);
  unload_texture(hex_icons[HEX_VIOLENCE]);
  unload_texture(hex_icons[HEX_DISCRIMINATION]);
  unload_texture(hex_icons[HEX_WAR]);


    unload_render_texture(target);
    unload_music_stream(music_dark_shrine_loop_meta.music);
    unload_music_stream(music_going_deep_meta.music);
    unload_music_stream(music_unforgiving_himalayas_meta.music);
    unload_music_stream(music_forest_meta.music);





    sb_free(&ending_sb);
    
    
    // TODO: Unload all loaded resources at this point

    close_audio_device();
    close_window();        // Close window and OpenGL context

    //--------------------------------------------------------------------------------------

    return 0;
}

//--------------------------------------------------------------------------------------------
// Module Functions Definition
//--------------------------------------------------------------------------------------------
// Update and draw frame
void update_and_draw_one_frame(void) {
  // Update
  //----------------------------------------------------------------------------------
  // TODO: Update variables / Implement example logic at this point
 
  frame_counter++;
  update_music_stream_ex(&music_dark_shrine_loop_meta);
  update_music_stream_ex(&music_going_deep_meta);
  update_music_stream_ex(&music_unforgiving_himalayas_meta);
  update_music_stream_ex(&music_forest_meta);

#ifdef _DEBUG
  if (is_key_down(KEY_LEFT_SHIFT)) {
    move_speed_of_controlled_values = 1.0f;
  } else {
    move_speed_of_controlled_values = 0.1f;
  }
  if (is_key_down(KEY_K)) {
    controlled_vector2.y -= move_speed_of_controlled_values;
  } else if (is_key_down(KEY_J)) {
    controlled_vector2.y += move_speed_of_controlled_values;
  } else if (is_key_down(KEY_H)) {
    controlled_vector2.x -= move_speed_of_controlled_values;
  } else if (is_key_down(KEY_L)) {
    controlled_vector2.x += move_speed_of_controlled_values;
  } else if (is_key_down(KEY_I)) {
    controlled_float += move_speed_of_controlled_values;
  } else if (is_key_down(KEY_O)) {
    controlled_float -= move_speed_of_controlled_values;
  }
#endif // _DEBUG





  //test_touch_points_count = get_touch_point_count();

  //for (size_t i = 0; i < test_touch_points_count; ++i) {
  //  test_touch_points[i] = get_touch_position(i);
  //}

  GameScreen screen = current_screen;

  input.pointer_position = get_mouse_position();
  input.pressed  = is_mouse_button_pressed (MOUSE_LEFT_BUTTON);
  input.down     = is_mouse_button_down    (MOUSE_LEFT_BUTTON);
  input.released = is_mouse_button_released(MOUSE_LEFT_BUTTON);
  update_hexagon_button(screen, &button.start_game);


#define UNROLL_BUTTONS_CALLBACK(BUTTON_NAME,_i, _p,_d,_r) update_hexagon_button(screen, &button.BUTTON_NAME); \
  if (button.BUTTON_NAME.pressed ) { button_##BUTTON_NAME##_on_pressed(); button.BUTTON_NAME.pressed=false; } \
  if (button.BUTTON_NAME.down    ) { button_##BUTTON_NAME##_on_down(); button.BUTTON_NAME.down=false; } \
  if (button.BUTTON_NAME.released) { button_##BUTTON_NAME##_on_released(); button.BUTTON_NAME.released=false; }
  HEX_BUTTONS_LIST(UNROLL_BUTTONS_CALLBACK)

    if ( black_amnesia_anim > -10.0f) {
      button.story_next.not_active = true;
    } else {
      button.story_next.not_active = false;
    }
    foreach (HexagonButton in draggable_buttons_on_grid) {
      (void)(item);
      update_hexagon_button(screen,current);
      if (current->pressed ) {
        if (grabbed_hex) {
          reset_grabbed();
        }
        grabbed_hex = current;
        current->pressed=false;
      }
      if (current->down    ) { current->down=false; }
      if (current->released) { current->released=false; }
    }
    

    if (input.released && grabbed_hex) {
      reset_grabbed();
    }

// :update
    switch (current_screen) {
      case SCREEN_LOGO: {
      } break;
      case SCREEN_TITLE: {
        if (!is_music_stream_playing(music_dark_shrine_loop_meta.music)) {
          start_music_and_show_it(music_dark_shrine_loop_meta);
        }
      } break;
      case SCREEN_GAMEPLAY: {
        init_stage_1();
        if (grabbed_hex) {
          grabbed_hex->target_position = input.pointer_position;
          //grabbed_hex->position = input.pointer_position;
        }
        //button.gameplay_draggable_hex_1.position = controlled_vector2;
      } break;
      case SCREEN_STORY: {
      } break;
      case SCREEN_BOOK: {
      } break;
      case SCREEN_BOOK_PAGE2: {
      } break;
      case SCREEN_SETTINGS: {
        //pause_all_music();
      } break;
      case SCREEN_ENDING: {
        update_ending();
      } break;
      default: UNREACHABLE("GameScreen unknown enum value"); break;
    }





  //----------------------------------------------------------------------------------

  // Draw
  //----------------------------------------------------------------------------------
  // Render game screen to a texture, 
  // it could be useful for scaling or further shader postprocessing
// :draw
  begin_texture_mode(target);
    clear_background(RAYWHITE);
    
    switch (current_screen) {
      case SCREEN_LOGO: {
        draw_rectangle(70, 90, 200, 200, BLACK);
        draw_rectangle(70 + 16, 90 + 16, 200 - 32, 200 - 32, RAYWHITE);
        draw_text("raylib", 70 + 200 - measure_text("raylib", 40) - 32, 90 + 200 - 40 - 24, 40, BLACK);

        draw_text("6.x", 290, 90 - 26, 280, BLACK);
        draw_text("GAMEJAM", 70, 90 + 210, 120, MAROON);

        if ((frame_counter/20)%2) draw_text("hex a merger", 160, 500, 50, BLACK);
        
        draw_rectangle_lines_ex((Rectangle){ 0, 0, screen_x_, screen_y_ }, 16, BLACK);

      } break;
      case SCREEN_TITLE: {
        draw_title_screen();
      } break;
      case SCREEN_GAMEPLAY: {
        draw_gameplay_screen();
      } break;
      case SCREEN_STORY: {
        current_screen = SCREEN_GAMEPLAY;
        draw_gameplay_screen();
        draw_rectangle(0,0,720,720, fade(BLACK,0.5f) );
        current_screen = SCREEN_STORY;
        draw_rectangle(0,720 - 260, 720, 260, BLACK );
//        draw_text(""
//"bla bla bla bla bla bla bla bla bla bla\n"
//"bla bla bla bla bla bla bla bla bla bla\n"
//"bla bla bla bla bla bla bla bla bla bla\n"
//"bla bla bla bla bla bla bla bla bla bla\n"
//"bla bla bla bla bla bla bla bla bla bla\n"
//"bla bla bla bla bla bla bla bla bla bla\n"
//"bla bla bla bla bla bla bla bla bla bla.\n"
//
//, 0, 720 - 240, 30, WHITE);

        if ( black_amnesia_anim > -10.0f) {
        } else {
          if (dialogue_frame.line) {
            draw_text(dialogue_frame.line, 10, 720 - 240, 30, WHITE);
          } else {
            draw_text("Nothing to say, sorry.", 10, 720 - 240, 30, WHITE);
          }
        }

        draw_left_character(dialogue_frame.character_left, dialogue_frame.character_speaking == C_LEFT);
        draw_right_character(dialogue_frame.character_right,  dialogue_frame.character_speaking == C_RIGHT);
        draw_all_buttons();
        if ( black_amnesia_anim > 0.0f) {
          black_amnesia_anim -= 0.1f;
          draw_circle(360,360,100.0f*black_amnesia_anim,BLACK);
        } else if ( black_amnesia_anim > -10.0f) {
          black_amnesia_anim -= 0.1f;
        }




      } break;
      case SCREEN_BOOK: {
        draw_book();
        current_screen = SCREEN_BOOK;
        draw_all_buttons();
        draw_texture_centered_ex(exit_texture,(Vector2){660.0f,60.0f}, 1.0f, fade(WHITE,1.0f));
      } break;
      case SCREEN_BOOK_PAGE2: {
      } break;
      case SCREEN_SETTINGS: {

        draw_texture(settings_background_texture, 0,0,WHITE);
        draw_all_buttons();
        
        draw_texture_centered_ex(settings_music_icon, button.settings_music_volume.position, settings.music_volume * 2.0f, WHITE);
        draw_texture_centered_ex(exit_texture,(Vector2){660.0f,60.0f}, 1.0f, fade(WHITE,1.0f));
      } break;
      case SCREEN_ENDING: {
        draw_ending();
      } break;
      default: UNREACHABLE("GameScreen unknown enum value"); break;
    }

    if (music_to_show.animation_timer > 0.0f) {
      music_to_show.animation_timer -= 0.1f;
      if (music_to_show.animation_timer > 20.0f) {
        draw_music_banner(0, 200 - (music_to_show.animation_timer*10.0f));
      } else if (music_to_show.animation_timer > 10.0f) {
        draw_music_banner(0, 0);
      } else {
        draw_music_banner(0, -100+(music_to_show.animation_timer*10.0f));
      }

    }

#ifdef _DEBUG
    draw_debug_overlay();
#endif
    // TODO: Draw your game screen here

    
  end_texture_mode();
  
  // Render to screen (main framebuffer)
  begin_drawing();
    clear_background(RAYWHITE);
    
    // Draw render texture to screen, scaled if required
    draw_texture_pro(target.texture, (Rectangle){ 0, 0, (float)target.texture.width, -(float)target.texture.height }, 
        (Rectangle){ 0, 0, (float)target.texture.width, (float)target.texture.height }, (Vector2){ 0, 0 }, 0.0f, WHITE);

    // TODO: Draw everything that requires to be drawn at this point, maybe UI?

  end_drawing();
  //----------------------------------------------------------------------------------  
}

static void next_line() {

  u play_line = current_dialogue_line;
  dialogue_frame = dia_main[play_line];
  current_dialogue_line = dialogue_frame.next_line;
  if (dialogue_frame.song != SONG_NULL ) {
    play_song(dialogue_frame.song);
    play_line = current_dialogue_line;
    dialogue_frame = dia_main[play_line];
    current_dialogue_line = dialogue_frame.next_line;
  }

  if (dialogue_frame.roll_credits) {
    button.gameplay_to_story.not_active = true;
    current_screen = SCREEN_ENDING;
    return;
  }

  if (!dialogue_frame.line) {
    current_screen = SCREEN_GAMEPLAY;

    if (dialogue_frame.wait_for != HEX_NULL) {
      if (dialogue_frame.after_wait_line == 0) {
        // reveal

        hex_reveal_rec(dialogue_frame.wait_for);
      } else {
        waiting_for = dialogue_frame.wait_for;
        after_wait_line = dialogue_frame.after_wait_line;
      }
    }
  }
}

static void update_hexagon_button(GameScreen screen, HexagonButton* self) {
  self->position = vector2_move_towards(self->position, self->target_position,fmaxf(vector2_distance(self->position, self->target_position)/10.0f,0.1f));

  if (self->active_on == screen) {
    bool in_range = check_collision_point_circle(input.pointer_position, self->position, self->radius - self->thickness) && !self->not_active;
    self->pressed  = input.pressed  && in_range;
    self->down     = input.down     && in_range;
    self->released = input.released && in_range;
    if (self->was_in_range && !in_range) {
      self->released = true;
    }
    self->was_in_range = in_range;
  }
  self->rotation += self->rotation_speed;
}


static Texture texture_from_file(const char* filename) {
  Image image = load_image(filename);
  Texture self = load_texture_from_image(image);
  unload_image(image);
  return self;
}

static void draw_raylib_logo(int x, int y) {
  draw_rectangle(x, y, 200, 200, BLACK);
  draw_rectangle(x + 16, y + 16, 200 - 32, 200 - 32, RAYWHITE);
  draw_text("raylib", x + 200 - measure_text("raylib", 40) - 32, y + 200 - 40 - 24, 40, BLACK);
}
// thickness goes in
static void draw_hexagon_outline(Vector2 position, float radius, float thickness, float rotation, Color color) {
  draw_ring(position, radius - thickness, radius, rotation, rotation + 360.0f, 6, color);
}
static void draw_hexagon_insides(Vector2 position, float radius, float rotation, Color color) {
  draw_ring(position, 0.0f, radius, rotation, rotation + 360.0f, 6, color);
}
static void draw_hexagon(Vector2 position, float radius, float thickness, float rotation, Color color_inside, Color color_outside) {
  draw_hexagon_insides(position, radius, rotation, color_inside);
  draw_hexagon_outline(position, radius, thickness, rotation, color_outside);
}
static void draw_all_buttons() {
#define UNROLL_BUTTONS_DRAW(BUTTON_NAME,_i, _p,_d,_r) draw_hexagon_button(button.BUTTON_NAME);
  HEX_BUTTONS_LIST(UNROLL_BUTTONS_DRAW)


  foreach (HexagonButton as btn in draggable_buttons_on_grid) {
    draw_hexagon_button(btn);
  }
  if (grabbed_hex) {
    draw_hexagon_button(*grabbed_hex);
  }
}
static void draw_gameplay_screen() {
  draw_texture(gameplay_background_texture, 0, 0, WHITE);
  draw_texture(char_hand[dialogue_frame.character_right], 0, 0, WHITE);
  draw_all_buttons();
  draw_texture_centered_ex(hex_icons[waiting_for], (Vector2){60.0f,60.0f}, 1.0f, fade(WHITE,1.0f));
  draw_texture_centered_ex(settings_texture,(Vector2){660.0f,60.0f}, 1.0f, fade(WHITE,1.0f));
  draw_texture_centered_ex(lens_texture,(Vector2){70.0f,480.0f}, 1.0f, fade(WHITE,1.0f));
  
}
static void draw_title_screen() {
  draw_texture(title_background_texture, 0,0,WHITE);

  draw_texture_centered_ex(settings_texture, button.options.position, 3.0f, WHITE);
  draw_text("Hex A Merge", 30, 30, 100, WEAK_BLUE);
  //draw_hexagon((Vector2){450.0f, 332.0f}, 175.0f, 10.0f, 0.0f, WHITE,  BLACK);

  draw_all_buttons();



  //draw_hexagon((Vector2){260.0f, 315.0f}, 175.0f, 10.0f, frame_counter, WHITE, BLACK);


  if ((frame_counter/20)%2) {
    draw_text("Press\nhere\nto play", 150, 222, 60, WEAK_BLUE);
  } else {
    draw_text("Press\nhere\nto play", 150, 222, 60, fade(WEAK_BLUE, 0.75f) );
  }

 // i think i dont need multitouch
 // for (size_t i = 0; i < test_touch_points_count; ++i) {
 //   draw_hexagon( test_touch_points[i], 100.0f, 10.0f, 0.0f, (Color){10.0f * i, 100.0f, 100.0f}, BLACK);
 // }

  draw_raylib_logo(70,500);
  draw_text("6.x", 290, 500 - 26, 280, BLACK);
  draw_text("GAMEJAM", 70 + 16 + 8, 500 + 16, 32, MAROON);
  draw_text("hex\nmerge", 70 + 16 + 8, 500 + 16 + 32, 32,DARKPURPLE  );
}
#ifdef _DEBUG
static void draw_debug_overlay() {
  draw_hexagon( input.pointer_position, 10.0f, 1.0f, 0.0f, fade( WEAK_BLUE, 0.2f), fade(BLACK, 0.2f));
  draw_rectangle(controlled_vector2.x, controlled_vector2.y, controlled_float, controlled_float, RED);
  draw_hexagon( controlled_vector2, controlled_float, 1.0f, 0.0f, fade( WEAK_BLUE, 0.2f), fade(BLACK, 0.2f));

  draw_text( text_format("(%f,%f)\n[%f]", controlled_vector2.x, controlled_vector2.y, controlled_float) , 0, 0, 10, BLACK);

  switch (current_screen) {
    case SCREEN_LOGO: {
      draw_text("SCREEN_LOGO", 10,100,20,BLACK);
    } break;
    case SCREEN_TITLE: {
      draw_text("SCREE_TITLE", 10,100,20,BLACK);
    } break;
    case SCREEN_GAMEPLAY: {
      draw_text("SCREEN_GAMEPLAY", 10,100,20,BLACK);
    } break;
    case SCREEN_STORY: {
      draw_text("SCREEN_STORY", 10,100,20,BLACK);
    } break;
    case SCREEN_BOOK: {
      draw_text("SCREEN_BOOK", 10,100,20,BLACK);
    } break;
    case SCREEN_BOOK_PAGE2: {
      draw_text("SCREEN_BOOK_PAGE2", 10,100,20,BLACK);
    } break;
    case SCREEN_SETTINGS: {
      draw_text("SCREEN_SETTINGS", 10,100,20,BLACK);
    } break;
    case SCREEN_ENDING: {
      draw_text("SCREEN_ENDING", 10,100,20,BLACK);
    } break;
    default: UNREACHABLE("GameScreen unknown enum value"); break;
  }
}
#endif

static void draw_hexagon_button(HexagonButton self) {
  if (self.active_on == current_screen && !self.not_active) {
    draw_hexagon(self.position, self.radius, self.thickness, self.rotation, self.color_inside, self.color_outside);
    if (self.has_hex) {
      if (self.hex.traits.count == 0) {
        // nothing to draw
      } else if (self.hex.traits.count == 1) {
        // known hex
        HexTrait hex = da_first(&self.hex.traits);
        if (hex_is_shown[hex] ) {
          draw_hex_trait( da_first(&self.hex.traits), self.position, 1.0f);
        } else {
        draw_hex_trait(HEX_NULL, self.position, 1.0f);
        }
      } else {
        // unknown hex
        draw_hex_trait(HEX_NULL, self.position, 1.0f);
      }
    }
  }
}



static void draw_hex_trait(HexTrait hex_trait, Vector2 position,  float scale) {
  draw_texture_centered_ex(hex_icons[hex_trait], position, scale, WHITE);
}



static HexagonButton button_init_hex(Vector2 position, HexTrait starting_hex) {
  HexagonButton self = button_init_default(SCREEN_GAMEPLAY, position, 60.0f);
  da_push(&self.hex.traits, starting_hex);
  self.has_hex = true;
  self.color_inside = PAPER_WHITE;
  return self;
}
static HexagonButton button_init_book(Vector2 position, HexTrait starting_hex) {
  HexagonButton self = button_init_hex(position, starting_hex);
  self.active_on = SCREEN_BOOK;
  return self;
}
static HexagonButton button_init_playable(Vector2 position) {
  HexagonButton self = button_init_default(SCREEN_GAMEPLAY, position, 60.0f);
  self.color_inside = fade(WHITE, 0.05f);
  self.color_outside = fade(BLACK, 0.05f);
  return self;
}
static HexagonButton button_init_color(GameScreen active_on, Vector2 position, float radius, Color color_inside, Color color_outside) {
  HexagonButton self = button_init_default(active_on, position, radius);
  self.color_inside = color_inside;
  self.color_outside = color_outside;
  return self;
}
static HexagonButton button_init_default(GameScreen active_on, Vector2 position, float radius) {
    return (HexagonButton){
      .active_on=active_on,
      .position=position,
      .target_position=position,
      .home_position=position,
      .radius=radius,
      .thickness=10.0f,
      .rotation=0.0f,
      .color_inside=WHITE,
      .color_outside=BLACK,
      .pressed=false,
      .down=false,
      .released=false,
      .rotation_speed=0.0f,
    };
}
static HexagonButton button_init_start_game_() {
  HexagonButton self = button_init_default(SCREEN_TITLE, (Vector2){260.0f, 315.0f}, 175.0f);
      self.active_on=SCREEN_TITLE;
      self.position=(Vector2){260.0f, 315.0f};
      self.radius=175.0f;
      self.thickness=10.0f;
      self.rotation=0.0f;
      self.color_inside=PAPER_WHITE;
      self.color_outside=BLACK;
      self.pressed=false;
      self.down=false;
      self.released=false;
      self.rotation_speed=1.0f;
      return self;
}
static void pause_all_music() {
    pause_music_stream(music_dark_shrine_loop_meta.music);
    pause_music_stream(music_going_deep_meta.music);
    pause_music_stream(music_unforgiving_himalayas_meta.music);
    pause_music_stream(music_forest_meta.music);
}
static void load_music_from_meta(MetaMusic *mm) {
    Music self = {0};
    self = load_music_stream(mm->filename);
    play_music_stream(self);
    pause_music_stream(self);
    set_music_volume(self, mm->volume);
    mm->music = self;
}
static void draw_texture_centered_ex(Texture texture, Vector2 position, float scale, Color tint) {
  Vector2 dimensions = (Vector2){texture.width, texture.height};
  Vector2 offset = vector2_scale( vector2_negate( dimensions ), scale/2);
  draw_texture_ex(texture, vector2_add( position, offset ), 0.0f, scale, tint);
}
static void update_music_stream_ex(MetaMusic* mm) {
  set_music_volume(mm->music, settings.music_volume);
  update_music_stream(mm->music);
}
static void start_music_and_show_it(MetaMusic mm) {
  pause_all_music();
  resume_music_stream(mm.music);
  music_to_show.music=mm;
  music_to_show.animation_timer=30.0f;
}
static void play_song(Song song) {
  switch (song) {
    case SONG_SHRINE: start_music_and_show_it(music_dark_shrine_loop_meta); break;
    case SONG_DEEP: start_music_and_show_it(music_going_deep_meta); break;
    case SONG_HIMA: start_music_and_show_it(music_unforgiving_himalayas_meta); break;
    case SONG_FOREST: {
                        start_music_and_show_it(music_forest_meta);
                        black_amnesia_anim = 10.0f;
    } break;
    default: break;
  }
}
static void draw_music_banner(int x, int y) {
  const char * to_write = text_format( "now playing: %s\nby: %s", music_to_show.music.title, music_to_show.music.author);
  Vector2 banner_dim = measure_text_ex(get_font_default(), to_write, 30, 3);

  draw_rectangle(x,y,10+banner_dim.x+10,10+banner_dim.y+10, GRAY);
  draw_text(to_write, x+10, y+10, 30, WEAK_BLUE);

}
static void on_release_playable(int x, int y) {
  if (grabbed_hex) {
    if (grabbed_hex->other) {
      // this is hex that was already placed
      HexagonButton* gh = grabbed_hex;
      reset_grabbed();
      HexagonButton* merge_into = get_mergable( G_(x,y) );
      gh->target_position = G_(x,y);
      gh->home_position = G_(x,y);
      if (merge_into==gh) {
        // we didnt move
        merge_into = NULL;
      }
      if (merge_into) {
        // space is taken, so we merge into owner
        hex_merge(&merge_into->hex, &gh->hex);
        if (merge_into->hex.traits.count == 1) {
          HexTrait h = da_first(&merge_into->hex.traits);
          //hex_is_known[h] = true;
          hex_reveal_rec(h);
        }
        merge_into->position = gh->position;
        // now scary part, we MUST pop ourselves.
        u position = (gh - draggable_buttons_on_grid.items);
        da_remove_at(&draggable_buttons_on_grid, position);


      } else {
        // space is empty, we are already in the array.
      }
    } else {
      // this is base hex
      HexagonButton to_push = *grabbed_hex;
      grabbed_hex->position = vector2_add( grabbed_hex->home_position, (Vector2){0.0f,240.0f} );
      if ( ran_out_of_hexes() ) {
        grabbed_hex->not_active = true;
      }
      // to_push now owns hex, so base hex needs to be regenerated
      new_hex(grabbed_hex);
      reset_grabbed();
      to_push.other = true;
      to_push.target_position = G_(x,y);
      to_push.home_position = G_(x,y);
      HexagonButton* merge_into = get_mergable( G_(x,y) );
      if (merge_into) {
        // space is taken, so we merge into owner
        hex_merge(&merge_into->hex, &to_push.hex);
        if (merge_into->hex.traits.count == 1) {
          HexTrait h = da_first(&merge_into->hex.traits);
          //hex_is_known[h] = true;
          hex_reveal_rec(h);
        }
        merge_into->position = to_push.position;
      } else {
        // space is empty, so we allocate ourselves
        da_push(&draggable_buttons_on_grid, to_push);
      }
    }
    // TODO: add hex placement logic

  }

}
static HexagonButton* get_mergable( Vector2 position ) {
  foreach (HexagonButton in draggable_buttons_on_grid) {
    (void)(item);
    if ( Vector2Equals(  current->home_position, position) ) {
      return current;
    }
  }
  return NULL;
}
static void init_stage_1() {
  button.gameplay_grid_0_0.not_active = false;
  button.gameplay_grid_1_0.not_active = true;
}
static void new_hex(HexagonButton* btn) {
  btn->hex = (Hex){0};
#ifdef    _DEBUG
    da_push(&btn->hex.traits, waiting_for);
    hex_is_shown[waiting_for] = true;
    return;
#endif // _DEBUG
  if (&button.gameplay_draggable_hex_1 == btn) {
    da_push(&btn->hex.traits, HEX_AGGRESION);
  } else if (&button.gameplay_draggable_hex_2 == btn) {
    da_push(&btn->hex.traits, HEX_BLIND);
  } else if (&button.gameplay_draggable_hex_3 == btn) {
    da_push(&btn->hex.traits, HEX_REVERSO);
  } else if (&button.gameplay_draggable_hex_4 == btn) {
    da_push(&btn->hex.traits, HEX_FLU);
  } else if (&button.gameplay_draggable_hex_5 == btn) {
    da_push(&btn->hex.traits, HEX_WEAK);
  } else {
    da_push(&btn->hex.traits, random_available_hex());
  }
}

static HexTrait random_available_hex() {
  HexTrait rng = 0;
  while (!hex_is_known[rng]) {
    rng = get_random_value(1,21);
  }
  return rng;
}
static bool ran_out_of_hexes() {
  return false;
  hex_merge(NULL,NULL);
}

static void reset_grabbed() {
      grabbed_hex->target_position = grabbed_hex->home_position;
      grabbed_hex = NULL;
}
static void draw_book() {
  current_screen = SCREEN_GAMEPLAY;
  draw_gameplay_screen();
  draw_rectangle(0,0,720,720, fade(BLACK,0.5f) );
  draw_rectangle(60,120,720-60*2,720-120*2, fade(BLACK,0.5f) );
}

static void show_contents(Hex hex) {
  for (int i = 0; i < 21; ++i) {
    set_light(i, false);
  }
  foreach(HexTrait as trait in hex.traits) {
    set_light(trait, true);
  }
}
static void set_light(HexTrait hex, bool light) {
  switch (hex) {
    case HEX_REVERSO: button.book_REVERSO_highlight.not_active = !light; break;
    case HEX_BLIND: button.book_BLIND_highlight.not_active = !light; break;
    case HEX_DEAF: button.book_DEAF_highlight.not_active = !light; break;
    case HEX_ANOSMIA: button.book_ANOSMIA_highlight.not_active = !light; break;
    case HEX_SOMATOSENSORY_LOSS: button.book_SOMATOSENSORY_LOSS_highlight.not_active = !light; break;
    case HEX_AGEUSIA: button.book_AGEUSIA_highlight.not_active = !light; break;
    case HEX_DEATH: button.book_DEATH_highlight.not_active = !light; break;
    case HEX_WEAK: button.book_WEAK_highlight.not_active = !light; break;
    case HEX_SILENCE: button.book_SILENCE_highlight.not_active = !light; break;
    case HEX_PROCRASTINATION: button.book_PROCRASTINATION_highlight.not_active = !light; break;
    case HEX_FAMINE: button.book_FAMINE_highlight.not_active = !light; break;
    case HEX_FLU: button.book_FLU_highlight.not_active = !light; break;
    case HEX_INSANITY: button.book_INSANITY_highlight.not_active = !light; break;
    case HEX_CANCER: button.book_CANCER_highlight.not_active = !light; break;
    case HEX_AMNESIA: button.book_AMNESIA_highlight.not_active = !light; break;
    case HEX_PLAGUE: button.book_PLAGUE_highlight.not_active = !light; break;
    case HEX_AGGRESION: button.book_AGGRESION_highlight.not_active = !light; break;
    case HEX_VIOLENCE: button.book_VIOLENCE_highlight.not_active = !light; break;
    case HEX_DISCRIMINATION: button.book_DISCRIMINATION_highlight.not_active = !light; break;
    case HEX_WAR: button.book_WAR_highlight.not_active = !light; break;
    default: break;
  }
}
static void show_recepie(HexTrait hex) {
  for (int i = 0; i < 21; ++i) {
    set_light(i, false);
  }

  HexTrait* ingridients = merge_crafts[hex];
  for (;*ingridients!=HEX_NULL;++ingridients) {
    set_light(*ingridients, true);
  }
}
static void draw_character(Character c, bool is_left, bool is_speaking) {
  Texture to_draw = char_app[c];
  int offset_x = 0;
  Color tint = WHITE;
  if (is_speaking) {
    offset_x = 0;
    tint = WHITE;
  } else {
    offset_x = 100;
    tint = GRAY;
  }

  int speaker_x = 0;
  int speaker_y = 720 - 260 - 468;
  if (is_left) {
    speaker_x = 0;
    offset_x = -offset_x;
  } else {
    speaker_x = 720 - 348;
  }
  draw_texture(to_draw, speaker_x + offset_x, speaker_y, tint);


  const char* name = get_char_name(c);
  Vector2 name_banner_dim = measure_text_ex(get_font_default(), name, 30, 3);
  int x_ = name_banner_dim.x + 10;
  int y_ = name_banner_dim.y + 10;
  int x = 0;
  int y = 720 - 260 - y_;
  if (is_left) {
    draw_rectangle(x, y, x_, y_, BLACK );
    draw_text(name, x+5,y+10,30, WHITE);
  } else {
    x=720 - x_;
    draw_rectangle(x, y, x_, y_, BLACK );
    draw_text(name, x+5, y+10,30, WHITE);
  }



}
static void draw_left_character(Character c, bool is_speaking) {
  draw_character(c, true, is_speaking);
}
static void draw_right_character(Character c, bool is_speaking) {
  draw_character(c, false, is_speaking);
}
static void hex_reveal_rec(HexTrait hex) {
  hex_is_shown[hex] = true;

  HexTrait* ingridients = merge_crafts[hex];
  for (;*ingridients!=HEX_NULL;++ingridients) {
    if (!hex_is_shown[*ingridients]) {
      hex_reveal_rec(*ingridients);
    }
  }
}

// :end

#define MUSIC_FMT(mm) \
  mm.title, \
  mm.author, \
  mm.license



static void sbw_push_music(int* ending_pos_y, StringBuilderWrapper* sbw, MetaMusic mm) {
  sb_push_fmt(&sbw->sb,
    "%s\n"
    "by %s \n"
    "Licensed under: %s"
    , MUSIC_FMT(mm)
    );
  Vector2 banner_dim = measure_text_ex(get_font_default(), sb_to_str(&sbw->sb), 30, 3);
  sbw->screen_dim.x = 260;
  sbw->screen_dim.y = *ending_pos_y;
  sbw->screen_dim.width = banner_dim.x + 10;
  sbw->screen_dim.height = banner_dim.y + 10;
  sbw->link = mm.link;
  *ending_pos_y += sbw->screen_dim.height ;
}

static void update_sbw(StringBuilderWrapper* sbw) {
  if ( check_collision_point_rec(input.pointer_position, sbw->screen_dim) ) {
    sbw->is_focused = true;
    if ( input.pressed ) {
      set_clipboard_text(sbw->link);
    }
    sbw->is_down = input.down;
  } else {
    sbw->is_focused = false;
  }
}
static void update_ending() {
  if (cinema) {
    if (input.pressed) {
      cinema=false;
    }
  } else {
    update_sbw(&music_dark_shrine_loop_meta_sb);
    update_sbw(&music_going_deep_meta_sb);
    update_sbw(&music_forest_meta_sb);
    update_sbw(&music_unforgiving_himalayas_meta_sb);



    if ( check_collision_point_rec(input.pointer_position, (Rectangle){500,500,200,200} ) ) {
      if ( input.pressed ) {
        set_clipboard_text("https://www.raylib.com/");
      }
    }

    if ( check_collision_point_rec(input.pointer_position, (Rectangle){0,0,240,720} ) ) {
      if ( input.pressed ) {
        cinema=true;
      }
    }

  }

}


static void sbw_draw(StringBuilderWrapper* sbw) {
  float intensity = 1.0f;
  if (sbw->is_down) {
    intensity = 1.5f;
  } else if (sbw->is_focused) {
    intensity = 2.0f;
  } else {
    intensity = 1.0f;
  }
  draw_rectangle_gradient_ex(sbw->screen_dim, fade(BLACK, intensity*0.1f), fade(BLACK, intensity*0.2), fade(BLACK, intensity*0.5), fade(BLACK, intensity*0.4));
  draw_text(
      sb_to_str(&sbw->sb)
      , sbw->screen_dim.x+5, sbw->screen_dim.y+5, 30, WHITE
      );
}

static void draw_ending() {
  if (cinema) {
    draw_texture(ending, 0, 0, WHITE);
  } else {
    draw_texture(ending, 0, 0, WHITE);
    draw_rectangle(0+240,0,720-240,720, fade(BLACK, 0.5f));
    //draw_text(REP99("credits credits credits\n"), 320, 0, 30, WHITE);


    if (ending_sb.count ==0) {
      sb_push_fmt(&ending_sb,
        "     >>>Credits<<<\n"
        );
      int endy = 0;
      sbw_push_music(&endy,&music_dark_shrine_loop_meta_sb ,music_dark_shrine_loop_meta);
      sbw_push_music(&endy,&music_going_deep_meta_sb,music_going_deep_meta);
      sbw_push_music(&endy,&music_forest_meta_sb,music_forest_meta);
      sbw_push_music(&endy,&music_unforgiving_himalayas_meta_sb,music_unforgiving_himalayas_meta);
    }
    sbw_draw(&music_dark_shrine_loop_meta_sb);
    sbw_draw(&music_going_deep_meta_sb);
    sbw_draw(&music_forest_meta_sb);
    sbw_draw(&music_unforgiving_himalayas_meta_sb);
    draw_raylib_logo(500,500);


    draw_text("Made with:", 260, 500 + 200 - 40 - 24, 40, WHITE);

    draw_text(
        "Press item in credits\n"
        "to copy link to"
        "\n clipboard\n\n"
        "Press here to hide\n"
        "credits\n"
        "(again to return)"
        , 10, 175, 20, BLACK);
  }

}

#define UNROLL_BUTTONS_DEFINE_FUNCS(BUTTON_NAME,INIT,PRESSED,DOWN,RELEASED) \
  static HexagonButton button_init_##BUTTON_NAME() INIT \
  static void button_##BUTTON_NAME##_on_pressed() PRESSED \
  static void button_##BUTTON_NAME##_on_down() DOWN \
  static void button_##BUTTON_NAME##_on_released() RELEASED
  HEX_BUTTONS_LIST(UNROLL_BUTTONS_DEFINE_FUNCS)
