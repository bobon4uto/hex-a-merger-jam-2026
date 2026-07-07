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



#define RESOURCES_IMPLEMENTATION
#include "resources.h"


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
#define WEAK_BLUE       CLITERAL(Color){0x55, 0x55,0xaa,0xff}     // Weak Blue

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
  float radius;
  float thickness;
  float rotation;
  Color color_inside;
  Color color_outside;
  bool pressed, down, released, other;
  float rotation_speed;
} HexagonButton;

typedef struct sMusicBanner {
  float animation_timer;
  MetaMusic music;
} MusicBanner;


// macro slop
  //                          bool pressed, down, released;
// :btn
#define HEX_BUTTONS_LIST(T) \
  T(gameplay_back_to_title, \
      { return button_init_default(SCREEN_GAMEPLAY, (Vector2){100.0f,100.0f}, 100.0f); }, \
      { current_screen = SCREEN_TITLE; },{},{} \
    ) \
  T(settings_back_to_title, \
      { return button_init_default(SCREEN_SETTINGS, (Vector2){100.0f,100.0f}, 100.0f); }, \
      { current_screen = SCREEN_TITLE; },{},{} \
    ) \
  T(settings_debug_music_start, \
      { return button_init_default(SCREEN_SETTINGS, (Vector2){400.0f,100.0f}, 100.0f); }, \
      { start_music_and_show_it(music_dark_shrine,music_dark_shrine_loop_meta); },{},{} \
    ) \
  T(settings_debug_music_start2, \
      { return button_init_default(SCREEN_SETTINGS, (Vector2){500.0f,100.0f}, 100.0f); }, \
      { start_music_and_show_it(music_going_deep,music_going_deep_meta); },{},{} \
    ) \
  T(settings_debug_music_stop, \
      { return button_init_default(SCREEN_SETTINGS, (Vector2){400.0f,300.0f}, 100.0f); }, \
      {pause_all_music(); },{},{} \
    ) \
  T(settings_music_volume, \
      { return button_init_default(SCREEN_SETTINGS, (Vector2){500.0f,500.0f}, 150.0f); }, \
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
      { return button_init_default(SCREEN_TITLE, (Vector2){450.0f,332.0f}, 175.0f); }, \
      { current_screen = SCREEN_SETTINGS; },{},{} \
    ) \
  T(start_game, \
      { return button_init_start_game_(); }, \
      { current_screen = SCREEN_GAMEPLAY; },{},{}\
    )

typedef struct sHexagonButtonMap {
#define UNROLL_BUTTONS_STRUCT(BUTTON_NAME,_i,_p,_d,_r) HexagonButton BUTTON_NAME;
  HEX_BUTTONS_LIST(UNROLL_BUTTONS_STRUCT)
} HexagonButtonMap;

// TODO: Define your custom data types here

//----------------------------------------------------------------------------------
// Global Variables Definition (local to this module)
//----------------------------------------------------------------------------------
static const int screen_x_ = 720;
static const int screen_y_ = 720;

static RenderTexture2D target = { 0 };  // Render texture to render our game
static int frame_counter = 0;
static GameScreen current_screen = SCREEN_TITLE;
static Settings settings = {0};





static HexagonButtonMap button = {0};


static Input input = {0};

// resources
static Texture title_background_texture = { 0 };
static Texture settings_music_icon = { 0 };
static Music music_dark_shrine = { 0 };
static Music music_going_deep = { 0 };
static MusicBanner music_to_show = {0};



static Vector2 controlled_vector2 = { 0 };
static float move_speed_of_controlled_values = 0.01f;

static float controlled_float = 0.00f;

// TODO: Define global variables here, recommended to make them static

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
// :funcs
static void update_and_draw_one_frame(void);      // Update and Draw one frame

static Texture texture_from_file(const char* filename);

static void update_hexagon_button(HexagonButton* self);
static void update_music_stream_ex(Music m);

static void start_music_and_show_it(Music m, MetaMusic mm);


static void draw_raylib_logo(int x, int y);
static void draw_music_banner(int x, int y);
static void draw_texture_centered_ex(Texture texture, Vector2 position, float scale, Color tint);
static void draw_hexagon_outline(Vector2 position, float radius, float thickness, float rotation, Color color);
static void draw_hexagon_insides(Vector2 position, float radius, float rotation, Color color);
static void draw_hexagon(Vector2 position, float radius, float thickness, float rotation, Color color_inside, Color color_outside);
static void draw_hexagon_button(HexagonButton self);
static void draw_title_screen();
static void draw_gameplay_screen();

static void draw_all_buttons();

static void draw_debug_overlay();

static HexagonButton button_init_default(GameScreen active_on, Vector2 position, float radius);
#define UNROLL_BUTTONS_INIT_FUNCS(BUTTON_NAME,_i,_p,_d,_r) \
  static HexagonButton button_init_##BUTTON_NAME(); \
  static void button_##BUTTON_NAME##_on_pressed(); \
  static void button_##BUTTON_NAME##_on_down(); \
  static void button_##BUTTON_NAME##_on_released();
  HEX_BUTTONS_LIST(UNROLL_BUTTONS_INIT_FUNCS)



static HexagonButton button_init_start_game_();
static Music load_music_from_meta(MetaMusic mm);
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


    init_audio_device();
    music_dark_shrine = load_music_from_meta(music_dark_shrine_loop_meta);
    music_going_deep = load_music_from_meta(music_going_deep_meta);

    // TODO: Load resources / Initialize variables at this point
#define UNROLL_BUTTONS_INIT(BUTTON_NAME,_i,_p,_d,_r) button.BUTTON_NAME = button_init_##BUTTON_NAME();
  HEX_BUTTONS_LIST(UNROLL_BUTTONS_INIT)

    settings.music_volume = 1.0f;

    title_background_texture = texture_from_file("resources/mockup/mockup_title_screen.png");
    settings_music_icon = texture_from_file("resources/sprites/music_icon.png");
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
    unload_texture(title_background_texture);
    unload_texture(settings_music_icon);
    unload_render_texture(target);
    unload_music_stream(music_dark_shrine);
    unload_music_stream(music_going_deep);
    
    
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
  update_music_stream_ex(music_dark_shrine);
  update_music_stream_ex(music_going_deep);

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

  input.pointer_position = get_mouse_position();
  input.pressed  = is_mouse_button_pressed (MOUSE_LEFT_BUTTON);
  input.down     = is_mouse_button_down    (MOUSE_LEFT_BUTTON);
  input.released = is_mouse_button_released(MOUSE_LEFT_BUTTON);
  update_hexagon_button(&button.start_game);


#define UNROLL_BUTTONS_CALLBACK(BUTTON_NAME,_i, _p,_d,_r) update_hexagon_button(&button.BUTTON_NAME); \
  if (button.BUTTON_NAME.pressed) { button_##BUTTON_NAME##_on_pressed(); button.BUTTON_NAME.pressed=false; } \
  if (button.BUTTON_NAME.down) { button_##BUTTON_NAME##_on_down(); button.BUTTON_NAME.down=false; } \
  if (button.BUTTON_NAME.released) { button_##BUTTON_NAME##_on_released(); button.BUTTON_NAME.released=false; }
  HEX_BUTTONS_LIST(UNROLL_BUTTONS_CALLBACK)



// :update
    switch (current_screen) {
      case SCREEN_LOGO: {
      } break;
      case SCREEN_TITLE: {
        if (!is_music_stream_playing(music_dark_shrine)) {
          controlled_float +=1.0f;
          resume_music_stream(music_dark_shrine);
        }
      } break;
      case SCREEN_GAMEPLAY: {
      } break;
      case SCREEN_STORY: {
      } break;
      case SCREEN_SETTINGS: {
        //pause_all_music();
      } break;
      case SCREEN_ENDING: {
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
      } break;
      case SCREEN_SETTINGS: {
        draw_all_buttons();
        
        draw_texture_centered_ex(settings_music_icon, button.settings_music_volume.position, settings.music_volume * 2.0f, WHITE);
      } break;
      case SCREEN_ENDING: {
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


static void update_hexagon_button(HexagonButton* self) {
  if (self->active_on == current_screen) {
    bool in_range = check_collision_point_circle(input.pointer_position, self->position, self->radius);
    self->pressed  = input.pressed  && in_range;
    self->down     = input.down     && in_range;
    self->released = input.released && in_range;
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
}
static void draw_gameplay_screen() {
  draw_all_buttons();
}
static void draw_title_screen() {
  draw_texture(title_background_texture, 0, 0, WHITE);
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
  draw_text("hex\nmegre", 70 + 16 + 8, 500 + 16 + 32, 32,DARKPURPLE  );
}
#ifdef _DEBUG
static void draw_debug_overlay() {
  draw_hexagon( input.pointer_position, 10.0f, 1.0f, 0.0f, fade( WEAK_BLUE, 0.2f), fade(BLACK, 0.2f));

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
  if (self.active_on == current_screen) {
    draw_hexagon(self.position, self.radius, self.thickness, self.rotation, self.color_inside, self.color_outside);
  }
}







static HexagonButton button_init_default(GameScreen active_on, Vector2 position, float radius) {
    return (HexagonButton){
      .active_on=active_on,
      .position=position,
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
    return (HexagonButton){
      .active_on=SCREEN_TITLE,
      .position=(Vector2){260.0f, 315.0f},
      .radius=175.0f,
      .thickness=10.0f,
      .rotation=0.0f,
      .color_inside=WHITE,
      .color_outside=BLACK,
      .pressed=false,
      .down=false,
      .released=false,
      .rotation_speed=1.0f,
    };
}
static void pause_all_music() {
    pause_music_stream(music_dark_shrine);
    pause_music_stream(music_going_deep);
}
static Music load_music_from_meta(MetaMusic mm) {
    Music self = {0};
    self = load_music_stream(mm.filename);
    play_music_stream(self);
    pause_music_stream(self);
    set_music_volume(self, mm.volume);
    return self;
}
static void draw_texture_centered_ex(Texture texture, Vector2 position, float scale, Color tint) {
  Vector2 dimensions = (Vector2){texture.width, texture.height};
  Vector2 offset = vector2_scale( vector2_negate( dimensions ), scale/2);
  draw_texture_ex(texture, vector2_add( position, offset ), 0.0f, scale, tint);
}
static void update_music_stream_ex(Music m) {
  set_music_volume(m, settings.music_volume);
  update_music_stream(m);
}
static void start_music_and_show_it(Music m, MetaMusic mm) {
  pause_all_music();
  resume_music_stream(m);
  music_to_show.music=mm;
  music_to_show.animation_timer=30.0f;
}
static void draw_music_banner(int x, int y) {
  const char * to_write = text_format( "now playing: %s\nby: %s", music_to_show.music.title, music_to_show.music.author);
  Vector2 banner_dim = measure_text_ex(get_font_default(), to_write, 30, 3);

  draw_rectangle(x,y,10+banner_dim.x+10,10+banner_dim.y+10, GRAY);
  draw_text(to_write, x+10, y+10, 30, WEAK_BLUE);

}


#define UNROLL_BUTTONS_DEFINE_FUNCS(BUTTON_NAME,INIT,PRESSED,DOWN,RELEASED) \
  static HexagonButton button_init_##BUTTON_NAME() INIT \
  static void button_##BUTTON_NAME##_on_pressed() PRESSED \
  static void button_##BUTTON_NAME##_on_down() DOWN \
  static void button_##BUTTON_NAME##_on_released() RELEASED
  HEX_BUTTONS_LIST(UNROLL_BUTTONS_DEFINE_FUNCS)
