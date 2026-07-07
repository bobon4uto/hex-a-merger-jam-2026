#ifdef _EXAMPLE_MAIN_

    #define NOU_STRING_BUILDER_IMPLEMENTATION
    #define NOU_STRING_VIEW_IMPLEMENTATION
    #include "no.util/string_builder.h"
    int main () {
      StringBuilder sb_empty = {0};
      sb_push_str(&sb_empty, "hello");
      StringBuilder sb_from_string = sb_from_str("str");
      (void)(sb_from_string);
      StringBuilder sb_from_f = sb_from_file("important.txt");
      printf("%.*s", SB_FMT(sb_from_f));
      return 0;
    }

#endif

#ifndef   NOU_STRING_BUILDER_H_
#define   NOU_STRING_BUILDER_H_
#include "types.h"
typedef struct sStringBuilder StringBuilder;
typedef struct sStringView StringView;
typedef struct sStringBuilder {
  u capacity;
  u count;
  str items;
} StringBuilder;
NOU(StringBuilder);
#include "dynamic_array.h"
#include "string_view.h"
void               sb_init(StringBuilder *sb);
bool               sb_is_equal       (StringBuilder left, StringBuilder right);
bool               sb_is_exact       (StringBuilder left, StringBuilder right);
StringBuilder      sb_push_c         (StringBuilder* sb, char c);
StringBuilder      sb_push_sized_str (StringBuilder* sb, cstr s, u size);
StringBuilder      sb_from_sized_str (cstr s, u size);
StringBuilder      sb_push_str       (StringBuilder* sb, cstr s);
StringBuilder      sb_from_str       (cstr s);
StringBuilder      sb_push_fmt       (StringBuilder* sb, cstr fmt, ...);
StringBuilder      sb_from_fmt       (cstr fmt, ...);
StringBuilder      sb_push_sv        (StringBuilder* sb, StringView sv);
StringBuilder      sb_from_sv        (StringView sv);
StringBuilder      sb_push_exec      (StringBuilder* sb, char** cmd);
StringBuilder      sb_from_exec      (char** cmd);
Error              sb_write_to_file  (StringBuilder sb, str filename);
Error              sb_push_file       (StringBuilder *sb, cstr filename);
StringBuilder      sb_from_file      (cstr filename);
ResOfStringBuilder sb_from_file_safe (cstr filename);
str sb_to_str(StringBuilder* sb);


#define SB_FMT(sb) (int)(sb).count, (sb).items

void sb_free(StringBuilder* sb);



#ifdef   NOU_STRING_BUILDER_IMPLEMENTATION



#ifndef da_free
#include "dynamic_array.h" // for da_free, da_pushb
#endif // da_free

#ifndef nou_log
#include <stdio.h> // for printf
#define nou_log printf
#endif // nou_log

#ifndef nou_fseek
#include <stdio.h> // for printf
#define nou_fseek(stream, offset, whence) fseek(stream, offset, whence)
#endif // nou_fseek

#ifndef nou_rewind
#include <stdio.h> // for rewind
#define nou_rewind(stream) rewind(stream)
#endif // nou_rewind

#ifndef nou_ftell
#include <stdio.h> // for ftell
#define nou_ftell(stream) ftell(stream)
#endif // nou_ftell

#ifndef nou_fread
#include <stdio.h> // for fread
#define nou_fread(ptr, size, n, stream) fread(ptr, size, n, stream)
#endif // nou_fread

#ifndef nou_fopen
#include <stdio.h> // for fopen
#define nou_fopen(filename, mode) fopen(filename, mode)
#endif // nou_fopen

#ifndef nou_fclose
#include <stdio.h> // for fclose
#define nou_fclose fclose
#endif // nou_fclose

#ifndef nou_strlen
#include <string.h> // for strlen
#define nou_strlen(s) strlen(s)
#endif // nou_strlen

#ifndef nou_va_list
#include <stdarg.h> // for va_list
#define nou_va_list va_list
#endif // nou_va_list

#ifndef nou_va_start
#include <stdarg.h> // for va_start
#define nou_va_start va_start
#endif // nou_va_start

#ifndef nou_va_end
#include <stdarg.h> // for va_end
#define nou_va_end va_end
#endif // nou_va_end

#ifndef nou_vsnprintf
#include <stdio.h> // for vsnprintf
#define nou_vsnprintf vsnprintf
#endif // nou_vsnprintf


#ifndef NOU_STDOUT_FILENO
#include <unistd.h> // for STDOUT_FILENO
#define NOU_STDOUT_FILENO STDOUT_FILENO
#endif // NOU_STDOUT_FILENO 

#ifndef nou_wait
#include <sys/wait.h> // for wait
#define nou_wait wait
#endif // nou_wait



#ifndef nou_pipe
#include <unistd.h> // for pipe
#define nou_pipe pipe
#endif // nou_pipe

#ifndef nou_fork
#include <unistd.h> // for fork
#define nou_fork fork
#endif // nou_fork

#ifndef nou_close
#include <unistd.h> // for close
#define nou_close close
#endif // nou_close

#ifndef nou_dup2
#include <unistd.h> // for dup2
#define nou_dup2 dup2
#endif // nou_dup2

#ifndef nou_close
#include <unistd.h> // for close
#define nou_close close
#endif // nou_close

#ifndef nou_execvp
#include <unistd.h> // for execvp
#define nou_execvp execvp
#endif // nou_execvp

#ifndef nou_read
#include <unistd.h> // for read
#define nou_read read
#endif // nou_read


void sb_init(StringBuilder* sb) {
  (void)(sb);
}

bool               sb_is_equal       (StringBuilder left, StringBuilder right) {
  return sv_is_equal( sv_from_sb(left), sv_from_sb(right) );
}
bool               sb_is_exact       (StringBuilder left, StringBuilder right) {
  return left.items==right.items && left.count==right.count;
}
StringBuilder      sb_push_c         (StringBuilder* sb, char c) {
  da_push(sb, c);
  return *sb;
}
StringBuilder      sb_push_sized_str (StringBuilder* sb, cstr s, u size) {
  da_pushb(sb, s, size);
  return *sb;
}
StringBuilder sb_from_sized_str(cstr s, u size) {
  StringBuilder sb = {0};
  sb_init(&sb);
  return sb_push_sized_str(&sb, s, size);
}

StringBuilder      sb_push_str       (StringBuilder* sb, cstr s) {
  return sb_push_sized_str(sb, s, nou_strlen(s));
}
StringBuilder sb_from_str(cstr s) {
  StringBuilder sb = {0};
  sb_init(&sb);
  return sb_push_str(&sb, s);
}
StringBuilder sb_push_fmt(StringBuilder *sb, cstr fmt, ...)
{

    nou_va_list args;

    nou_va_start(args, fmt);
    int n = nou_vsnprintf(NULL, 0, fmt, args);
    nou_va_end(args);

    da_reserve(sb, sb->count + n + 1);
    char *dest = sb->items + sb->count;
    nou_va_start(args, fmt);
    nou_vsnprintf(dest, n+1, fmt, args);
    nou_va_end(args);

    sb->count += n;

    return *sb;
}
StringBuilder sb_from_fmt(cstr fmt, ...) {
    StringBuilder sb0 = {0};
    StringBuilder *sb = &sb0;
    sb_init(sb);
    va_list args;

    nou_va_start(args, fmt);
    int n = nou_vsnprintf(NULL, 0, fmt, args);
    nou_va_end(args);

    da_reserve(sb, sb->count + n + 1);
    char *dest = sb->items + sb->count;
    nou_va_start(args, fmt);
    nou_vsnprintf(dest, n+1, fmt, args);
    nou_va_end(args);

    sb->count += n;

    return *sb;
}

Error sb_push_file(StringBuilder *sb, cstr filename) {
  FILE * f =nou_fopen(filename, "r");
  if (!f) {
    nou_log("file not read");
    return SB_PUSHFILE_FILE_NOT_READ;
  }
  int ret = nou_fseek(f, 0, SEEK_END);
  long end_of_file = nou_ftell(f);
  nou_rewind(f); // its rewind time!
  da_reserve(sb, sb->count + end_of_file);
  ret = nou_fread(sb->items + sb->count, sizeof( *(sb->items) ), sb->capacity, f);
  sb->count = sb->capacity;
  if (ret != end_of_file) {
    nou_log("didnt read the whole file");
    return SB_PUSHFILE_FILE_NOT_READ_FULLY;
  }
  nou_fclose(f);
  return OK;
}
ResOfStringBuilder sb_from_file_safe(cstr filename) {
  ResOfStringBuilder rsb = {0};
  sb_init(&(rsb.ok));
  rsb.err = sb_push_file(&(rsb.ok), filename);
  return rsb;
}
StringBuilder sb_from_file(cstr filename) {
  ResOfStringBuilder rsb = sb_from_file_safe(filename);
  sb_init(&(rsb.ok));
  return rsb.ok;
}
str sb_to_str(StringBuilder* sb) {
  if (sb->capacity > sb->count + 1) {
    if (sb->items[sb->count]=='\0') {
      return sb->items;
    }
  }
  sb_push_c(sb, '\0');
  return sb->items;
}

StringBuilder      sb_push_sv        (StringBuilder* sb, StringView sv) {
  return sb_push_sized_str(sb, sv.items, sv.count);
}
StringBuilder      sb_from_sv        (StringView sv) {

  StringBuilder sb = {0};
  sb_init(&sb);
  return sb_push_sv(&sb, sv);
}


StringBuilder   sb_push_exec      (StringBuilder* sb, char** cmd) {
  int return_status=0;
  int fdes[2] = {0};
  if (nou_pipe(fdes)==-1) return sb_from_str("");
  if (nou_fork()==0) {
    nou_close(fdes[0]);
    nou_dup2(fdes[1], NOU_STDOUT_FILENO);
    nou_close(fdes[1]);
    nou_execvp(cmd[0], cmd);
  }
  nou_wait(&return_status);
  nou_close(fdes[1]);
  char b[2048] = {0};
  ssize_t n;
  while ( (n=nou_read(fdes[0],b, sizeof(b) )) > 0 ) {
    da_pushb(sb,b, (size_t)n);
  }
  return *sb;
}
StringBuilder sb_from_exec(char** cmd) {
  StringBuilder sb ={0};
  sb_push_exec(&sb, cmd);
  return sb;
}
Error              sb_write_to_file  (StringBuilder sb, str filename) {
  return sv_write_to_file(sv_from_sb(sb), filename);
}
void sb_free(StringBuilder* sb) {
  da_free(sb);
}




#endif // NOU_STRING_BUILDER_IMPLEMENTATION





#endif // NOU_STRING_BUILDER_H_


