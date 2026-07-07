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
#include "raylib.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>      // Emscripten library
#endif

#include <stdio.h>                          // Required for: printf()
#include <stdlib.h>                         // Required for: 
#include <string.h>                         // Required for:

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
#define UNREAHCABLE(...) GENERAL_PANIC("UNREACHABLE", __VA_ARGS__)

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum { 
    SCREEN_LOGO = 0, 
    SCREEN_TITLE, 
    SCREEN_GAMEPLAY, 
    SCREEN_ENDING
} GameScreen;


typedef struct sHexagonButton {
  GameScreen active_on;
  Vector2 position;
  float radius;
  float thickness;
  float rotation;
  Color color_inside;
  Color color_outside;
  bool is_pressed;
  float rotation_speed;
} HexagonButton;

// TODO: Define your custom data types here

//----------------------------------------------------------------------------------
// Global Variables Definition (local to this module)
//----------------------------------------------------------------------------------
static const int screen_x_ = 720;
static const int screen_y_ = 720;

static RenderTexture2D target = { 0 };  // Render texture to render our game
static int frame_counter = 0;
static GameScreen current_screen = SCREEN_TITLE;

static Vector2 mouse_position = { 0 };



static HexagonButton button_start_game = { 0 };

// resources
static Texture title_background_texture = { 0 };




static Vector2 controlled_vector2 = { 0 };
static float move_speed_of_controlled_values = 0.01f;

static float controlled_float = 0.00f;

// TODO: Define global variables here, recommended to make them static

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static void update_and_draw_one_frame(void);      // Update and Draw one frame

static Texture texture_from_file(const char* filename);

static void update_hexagon_button(HexagonButton* self);


static void draw_raylib_logo(int x, int y);
static void draw_hexagon_outline(Vector2 position, float radius, float thickness, float rotation, Color color);
static void draw_hexagon_insides(Vector2 position, float radius, float rotation, Color color);
static void draw_hexagon(Vector2 position, float radius, float thickness, float rotation, Color color_inside, Color color_outside);
static void draw_hexagon_button(HexagonButton self);
static void draw_title_screen();
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
    

    // TODO: Load resources / Initialize variables at this point
    button_start_game = (HexagonButton){
      .active_on=SCREEN_TITLE,
      .position=(Vector2){260.0f, 315.0f},
      .radius=175.0f,
      .thickness=10.0f,
      .rotation=0.0f,
      .color_inside=WHITE,
      .color_outside=BLACK,
      .is_pressed=false,
      .rotation_speed=1.0f,
    };

    title_background_texture = texture_from_file("resources/mockup/mockup_title_screen.png");
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
    unload_render_texture(target);
    
    // TODO: Unload all loaded resources at this point

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

  // DEBUG
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




  if (is_cursor_on_screen() && is_mouse_button_down(MOUSE_LEFT_BUTTON)) {
    mouse_position = get_mouse_position();
  }
  update_hexagon_button(&button_start_game);


  if (button_start_game.is_pressed) {
    current_screen = SCREEN_GAMEPLAY;
  }
  //----------------------------------------------------------------------------------

  // Draw
  //----------------------------------------------------------------------------------
  // Render game screen to a texture, 
  // it could be useful for scaling or further shader postprocessing
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
      } break;
      case SCREEN_ENDING: {
      } break;
      default: UNREAHCABLE("GameScreen unknown enum value"); break;
    }
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
    self->is_pressed = check_collision_point_circle(mouse_position, self->position, self->radius);
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
static void draw_title_screen() {
  draw_texture(title_background_texture, 0, 0, WHITE);
  draw_text("Hex A Merge", 30, 30, 100, WEAK_BLUE);
  draw_hexagon((Vector2){450.0f, 332.0f}, 175.0f, 10.0f, 0.0f, WHITE,  BLACK);
  draw_hexagon_button(button_start_game);
  //draw_hexagon((Vector2){260.0f, 315.0f}, 175.0f, 10.0f, frame_counter, WHITE, BLACK);

  draw_text( text_format("(%f,%f)\n[%f]", controlled_vector2.x, controlled_vector2.y, controlled_float) , 0, 0, 10, BLACK);

  if ((frame_counter/20)%2) {
    draw_text("Press\nhere\nto play", 150, 222, 60, WEAK_BLUE);
  } else {
    draw_text("Press\nhere\nto play", 150, 222, 60, fade(WEAK_BLUE, 0.75f) );
  }

  draw_raylib_logo(70,500);
  draw_text("6.x", 290, 500 - 26, 280, BLACK);
  draw_text("GAMEJAM", 70 + 16 + 8, 500 + 16, 32, MAROON);
  draw_text("hex\nmegre", 70 + 16 + 8, 500 + 16 + 32, 32,DARKPURPLE  );
}

static void draw_hexagon_button(HexagonButton self) {
  if (self.active_on == current_screen) {
    draw_hexagon(self.position, self.radius, self.thickness, self.rotation, self.color_inside, self.color_outside);
  }
}

