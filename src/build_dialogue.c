#define NOU_STRING_BUILDER_IMPLEMENTATION
#define NOU_STRING_VIEW_IMPLEMENTATION
#include "no.util/core.h"
#include <raylib.h>
#include "snake_case_api.h"

bool is_alphanumeric(char c) {
  bool lower  = c>='a'&&c<='z';
  bool upper  = c>='A'&&c<='Z';
  bool number = c>='0'&&c<='9';
  bool underscore= c=='_';
  return lower||upper||number||underscore;
}

bool is_delim (char c) {
  return !is_alphanumeric(c);
}
u delim(StringView sv) {
  if ( sv_first(sv)==' ' || sv_first(sv)=='\n' ) {
    return 1;
  }
  return 0;
}
 char* cmd_char     = "char";
 char* cmd_rsay     = "rsay";
 char* cmd_lsay     = "lsay";
 char* cmd_rloop    = "rloop";
 char* cmd_wait_for = "wait_for";
 char* cmd_wait_end = "wait_end";
 char* cmd_reveal_hex = "reveal_hex";
 char* cmd_repeat_last = "repeat_last";
 char* cmd_roll_credits = "roll_credits";
 char* cmd_to_song = "to_song";
 char* cmd_stop = "STOP";

bool is_cmd(StringView item) {
  return   sv_is_equal_str(item, cmd_char)
        || sv_is_equal_str(item, cmd_rsay)
        || sv_is_equal_str(item, cmd_lsay)
        || sv_is_equal_str(item, cmd_rloop)
        || sv_is_equal_str(item, cmd_wait_for)
        || sv_is_equal_str(item, cmd_reveal_hex)
        || sv_is_equal_str(item, cmd_wait_end)
        || sv_is_equal_str(item, cmd_repeat_last)
        || sv_is_equal_str(item, cmd_roll_credits)
        || sv_is_equal_str(item, cmd_to_song)
        || sv_is_equal_str(item, cmd_stop)
        ;
}

int main() {

  set_trace_log_level(LOG_NONE);
  init_window(1,1,"a");
  StringBuilder dialogue_src_sb = sb_from_file("dialogue.txt");
  StringView dialogue_src = sv_from_sb(dialogue_src_sb);
  StringBuilder dialogue_c = {0};


  DA_(StringView) dialogue_w_spaces = sv_splitl(dialogue_src, delim);


  DA_(StringView) dialogue = {0};
  foreach(StringView in dialogue_w_spaces) {
    char c = sv_first(item);
    if (c == ' '|| c == '\n') {
      continue;
    }
    da_push(&dialogue, item);
  }
  da_free(&dialogue_w_spaces);
  const u limit = 500;
  u current_limit = 0;
  bool need_to_close = false;

  StringView last_say = {0};
  u lines_limit = 6;
  u lines = 0;

  foreach(StringView in dialogue) {
    if (is_cmd(item)) {
      if (need_to_close) {
      sb_push_fmt(&dialogue_c,"\");\n");
      }
      need_to_close = false;
    }

    if (sv_is_equal_str(item, cmd_char) ) {
      StringView character = *(current+1);
      sb_push_fmt(&dialogue_c,"dis->right_char = CHAR_%.*s;\n", SV_FMT(character));
      current+=1;
      continue;
    } else if (sv_is_equal_str(item, cmd_rsay) || sv_is_equal_str(item, cmd_lsay)|| sv_is_equal_str(item, cmd_rloop) ) {
      sb_push_fmt(&dialogue_c,"%.*s(dis,\"", SV_FMT(item));

      current_limit = 0;
      lines = 0;
      last_say = item;
      need_to_close = true;
      continue;

    } else if ( sv_is_equal_str(item, cmd_wait_for) || sv_is_equal_str(item, cmd_reveal_hex)) {
      StringView hex = *(current+1);
      sb_push_fmt(&dialogue_c,"%.*s(dis, HEX_%.*s);\n", SV_FMT(item), SV_FMT(hex));
      current+=1;
      continue;
    } else if ( sv_is_equal_str(item, cmd_wait_end) ||sv_is_equal_str(item, cmd_repeat_last) ||sv_is_equal_str(item, cmd_roll_credits) ) {
      sb_push_fmt(&dialogue_c,"%.*s(dis);\n", SV_FMT(item));
      continue;
    } else if ( sv_is_equal_str(item, cmd_to_song) ) {
      StringView song = *(current+1);
      sb_push_fmt(&dialogue_c,"to_song(dis, SONG_%.*s);\n", SV_FMT(song));
      current+=1;
      continue;
    } else if ( sv_is_equal_str(item, cmd_stop) ) {
      sb_push_fmt(&dialogue_c,"stop(dis);\n");
      continue;
    }


    u len = measure_text(text_format(" %.*s", SV_FMT(item)), 30);
    if (current_limit + len >= limit) {
      current_limit = 0;
      if (lines+1>=lines_limit) {
        lines = 0;
        *(current - 1) = last_say;
        current -= 2;
        continue;
      } else {
        lines+=1;
        sb_push_fmt(&dialogue_c,"\\n");
      }
      len = measure_text(text_format("%.*s", SV_FMT(item)), 30);
      current_limit += len;
    } else {
      current_limit += len;
      sb_push_fmt(&dialogue_c," ");
    }
    sb_push_fmt(&dialogue_c,"%.*s",SV_FMT(item));
  }
  if (need_to_close) {
      sb_push_fmt(&dialogue_c,"\");\n");
  }

  da_free(&dialogue);


  //printf("%.*s", SB_FMT(dialogue_c) );
  sb_write_to_file(dialogue_c, "dialogue.c" );

  sb_free(&dialogue_c);
  sb_free(&dialogue_src_sb);
  close_window();

}
