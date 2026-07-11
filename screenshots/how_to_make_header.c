
// this in while (!window_should_close())
      begin_drawing();
      Color table_color = (Color){0x36,0x23,0x1c, 0xFF};
      Color bg_color = (Color){0x1b,0x14,0x0b, 0xFF};
      clear_background( table_color );
      draw_texture(char_app[CHAR_MERSEUS], 630 - char_app[CHAR_MERSEUS].width,500 - char_app[CHAR_MERSEUS].height,WHITE);


      draw_hexagon((Vector2){140,120}, 60, 10, -10.0f, PAPER_WHITE, BLACK);
      draw_texture_centered_ex(hex_icons[HEX_AGGRESION], (Vector2){140,120}, 1.0f, WHITE);


      draw_hexagon((Vector2){180,160}, 60, 10, -20.0f, PAPER_WHITE, BLACK);
      draw_texture_centered_ex(hex_icons[HEX_PROCRASTINATION], (Vector2){180,160}, 1.0f, WHITE);

      draw_hexagon((Vector2){100,180}, 60, 10, 0.0f, PAPER_WHITE, BLACK);
      draw_texture_centered_ex(hex_icons[HEX_DEAF], (Vector2){100,180}, 1.0f, WHITE);

      draw_hexagon((Vector2){60,100}, 60, 10, 10.0f, PAPER_WHITE, BLACK);
      draw_texture_centered_ex(hex_icons[HEX_FLU], (Vector2){60,100}, 1.0f, WHITE);

      draw_text("=", 250,140, 30,PAPER_WHITE);

      draw_hexagon((Vector2){340,155}, 60, 10, 0.0f, PAPER_WHITE, BLACK);
      draw_texture_centered_ex(hex_icons[HEX_WAR], (Vector2){340,155}, 1.0f, WHITE);

      draw_raylib_logo(20,500 - 220);
        end_drawing();
