#ifdef _EXAMPLE_MAIN_

    #define NOU_STRING_VIEW_IMPLEMENTATION
    #include "no.util/string_view.h"
    size_t is_spacel(StringView sub_sv) {
      if ( sv_first(sub_sv) == ' ' ) {
        return 1;
      }
      return 0;
    }
    size_t is_spacer(StringView sub_sv) {
      if ( sv_last(sub_sv) == ' ' ) {
        return 1;
      }
      return 0;
    }
    int main() {
      StringView sv_empty = {0};
      printf("%.*s", SV_FMT(sv_empty));
      StringView sv_from = sv_from_str("how are you doing");
      StringView sv_choppedl1 = sv_chopl(&sv_from, is_spacel);
      StringView sv_choppedl2 = sv_chopl(&sv_from, is_spacel);
      StringView sv_choppedr1 = sv_chopr(&sv_from, is_spacer);
      StringView sv_choppedr2 = sv_chopr(&sv_from, is_spacer);
      printf("%.*s|%.*s|%.*s|%.*s|%.*s",
          SV_FMT(sv_choppedl1), SV_FMT(sv_choppedl2), SV_FMT(sv_from),
          SV_FMT(sv_choppedr2), SV_FMT(sv_choppedr1) );
      return 0;
    }

#endif // _EXAMPLE_MAIN_

#ifndef   NOU_STRING_VIEW_H_
#define   NOU_STRING_VIEW_H_
#include "types.h"

typedef struct sStringView {
  u count; // the order allows to do this: printf("%.*s", sv)
  str items; // But it is better to use SV_FMT macro
} StringView;
NOU(StringView);
StringView      sv_from_str       (str s);
StringView      sv_from_sized_str       (str s, u size);
StringView sv_from_range(str start, str end);
StringView sv_from_sv_range(StringView start, StringView end);
StringView sv_from_sv_range_inner(StringView start, StringView end);
StringView sv_from_sv_ends(StringView start, StringView end);
StringView      sv_bump_start(StringView sv, u bump); // bumps start, keeps end in the same place
StringView      sv_bump_end(StringView sv, u bump);
#define SV_FMT(sv) (int)(sv).count, (sv).items
bool sv_is_equal(StringView left, StringView right);
bool sv_starts_with_sv(StringView sv, StringView starts_with);
bool sv_is_equal_str(StringView sv, str s);
str sv_str_at(StringView  sv, u i);
char sv_char_at(StringView  sv, u i);
str sv_start (StringView sv);
str sv_end   (StringView sv);
char sv_first (StringView sv);
char sv_last  (StringView sv);

typedef u(*CmpFn)(StringView);
StringView      sv_peekl(StringView  sv, CmpFn cmp);
StringView      sv_peekr(StringView  sv, CmpFn cmp);
StringView      sv_chopr(StringView* sv, CmpFn cmp);
StringView      sv_chopl(StringView* sv, CmpFn cmp);




#ifdef NOU_STRING_VIEW_IMPLEMENTATION

#ifndef nou_strlen
#include <string.h> // for strlen
#define nou_strlen(s) strlen(s)
#endif // nou_strlen

StringView      sv_from_str       (str s) {
  return sv_from_sized_str(s, nou_strlen(s) );
}
StringView      sv_from_sized_str       (str s, u size) {
  StringView sv = {0};
  sv.items = s;
  sv.count = size;
  return sv;
}
StringView sv_from_range(str start, str end) {
  StringView sv = {0};
  sv.items = start;
  sv.count = end - start;
  return sv;
}
StringView sv_from_sv_range(StringView start, StringView end) {
  return sv_from_range( start.items, end.items+end.count );
}
StringView sv_from_sv_range_inner(StringView start, StringView end) {
  return sv_from_range( start.items+start.count, end.items );
}
StringView sv_from_sv_ends(StringView start, StringView end) {
  return sv_from_range( start.items+start.count, end.items+end.count );
}
StringView      sv_bump_start(StringView sv, u bump) {
  sv.items += bump;
  sv.count -= bump;
  return sv;
}
StringView      sv_bump_end(StringView sv, u bump) {
  sv.count -= bump;
  return sv;
}


str sv_start (StringView sv) {
  return sv.items;
}
str sv_end   (StringView sv) {
  if (sv.count == 0) return sv.items;
  // ? not sure how to deal with it, for len 1 it makes sense. len 0 doesnt
  return sv.items + sv.count - 1;
}
char sv_first (StringView sv) {
  return *sv_start(sv);
}
char sv_last  (StringView sv) {
  return *sv_end(sv);
}

StringView      sv_peekl(StringView  sv, CmpFn cmp) {
  for (str ip = sv.items; ip<sv.items+sv.count; ++ip) {
    u cmpres = cmp( sv_bump_start(sv, ip - sv.items) );
    if (cmpres > 0) {
      size_t c=ip-sv.items;
      str s=sv.items;
      if (c==0) return sv_from_sized_str(s,cmpres); // return the whole delim bacically
      return sv_from_sized_str(s,c);
    }
  }
  return sv;
}

StringView      sv_peekr(StringView  sv, CmpFn cmp) {
  str end=sv.items+sv.count-1;
  for (str ip = end; ip>=sv.items; --ip) {
    u cmpres = cmp( sv_from_sized_str( sv.items, ip-sv.items+1 ) );
    if (cmpres > 0) {
      u c=end-ip;
      if (c==0)return sv_from_sized_str(ip,1);
      str s=ip+1;
      return sv_from_sized_str(s,c);
    }
  }
  return sv;
}

StringView      sv_chopr(StringView* sv, CmpFn cmp) {
  StringView to_chop = sv_peekr(*sv, cmp);
  *sv = sv_bump_end(*sv, to_chop.count);
  return to_chop;
}
StringView      sv_chopl(StringView* sv, CmpFn cmp) {
  StringView to_chop = sv_peekl(*sv, cmp);
  *sv = sv_bump_start(*sv, to_chop.count);
  return to_chop;
}

bool sv_is_equal(StringView left, StringView right) {
  if (left.count != right.count) {
    return false;
  }
  return 0==strncmp(left.items, right.items, left.count);
}

bool sv_starts_with_sv(StringView sv, StringView starts_with) {
  if (sv.count < starts_with.count) {
    return false;
  } else {
    sv.count = starts_with.count;
    return sv_is_equal(sv, starts_with);
  }
}

bool sv_is_equal_str(StringView sv, str s) {
  return sv_is_equal(sv, sv_from_str(s));
}
str sv_str_at(StringView  sv, u i) {
  return i<sv.count ? sv.items+i : NULL;
}
char sv_char_at(StringView  sv, u i) {
  str s = sv_str_at(sv, i);
  return s!=NULL ? *s : '\0';
}

#endif // NOU_STRING_VIEW_IMPLEMENTATION




#endif // NOU_STRING_VIEW_H_

#ifdef NOU_STRING_BUILDER_H_
#ifndef NOU_STRING_VIEW_NOU_STRING_BUILDER_INTERLOP_H_
#define NOU_STRING_VIEW_NOU_STRING_BUILDER_INTERLOP_H_
StringView      sv_from_sb       (StringBuilder sb);
Error           sv_write_to_file (StringView sv, str filename);
#ifdef NOU_STRING_VIEW_IMPLEMENTATION
StringView      sv_from_sb       (StringBuilder sb) {
  return sv_from_sized_str(sb.items, sb.count);
}
#ifndef nou_fopen
#include <stdio.h> // for fopen
#define nou_fopen(filename, mode) fopen(filename, mode)
#endif // nou_fopen

#ifndef nou_fclose
#include <stdio.h> // for fclose
#define nou_fclose fclose
#endif // nou_fclose

#ifndef nou_fwrite
#include <stdio.h> // for fwrite
#define nou_fwrite fwrite
#endif // nou_fwrite

#ifndef nou_ferror
#include <stdio.h> // for ferror
#define nou_ferror ferror
#endif // nou_ferror

#ifndef NouFILE
#include <stdio.h> // for FILE
#define NouFILE FILE
#endif // NouFILE

Error           sv_write_to_file (StringView sv, str filename) {
    NouFILE *f = nou_fopen(filename, "wb");
    if (f == NULL) {
      return NOU_OPEN_FILE_FAILED;
    }
    while (sv.count > 0) {
      //fwrite
     //size_t fwrite( sv.iten, size, n, stream);
        size_t n = nou_fwrite(sv.items, 1, sv.count, f);
        if (nou_ferror(f)) {
          return SV_WRITE_TO_FILE_FAILED;
        }
        sv = sv_bump_start(sv, n);
    }

    nou_fclose(f);
  return OK;
}
#endif // NOU_STRING_VIEW_IMPLEMENTATION
#endif // NOU_STRING_BILDER_H_
#endif  // NOU_STRING_VIEW_NOU_STRING_BUILDER_INTERLOP_H_



#ifdef    DYNAMIC_ARRAY_H_
#ifndef     NOU_STRING_VIEW_DYNAMIC_ARRAY_INTERLOP_H_
#define     NOU_STRING_VIEW_DYNAMIC_ARRAY_INTERLOP_H_

typedef DA_(StringView) StringViewArray;
DA_(StringView) sv_splitl(StringView sv, CmpFn cmp);
DA_(StringView) sv_splitr(StringView sv, CmpFn cmp);

u sv_get_index(StringView self, DA_(StringView) svs);
StringView* sv_get_pointer(StringView self, DA_(StringView) svs);
bool sv_is_in(StringView self, DA_(StringView) svs);

DA_(StringView) svs_from_strs(str* str_src);
u sv_match_from(DA_(StringView) svs, StringView open_sv, StringView close_sv, u i);
u sv_match(DA_(StringView) svs, StringView open_sv, StringView close_sv);

#ifdef        NOU_STRING_VIEW_IMPLEMENTATION

DA_(StringView) sv_splitl(StringView sv, CmpFn cmp) {
  DA_(StringView) svs_split = {0};
  while (sv.count > 0) {
    da_push(&svs_split, sv_chopl(&sv, cmp));
  }
  return svs_split;
}
DA_(StringView) sv_splitr(StringView sv, CmpFn cmp) {
  DA_(StringView) svs_split = {0};
  while (sv.count > 0) {
    da_push(&svs_split, sv_chopr(&sv, cmp));
  }
  return svs_split;
}


u sv_get_index(StringView self, DA_(StringView) svs) {
  u i = 0;
  foreach (StringView as other in svs) {
    if (sv_is_equal(self, other)) {
      return i;
    }
    i += 1;
  }
  return i;
}
StringView* sv_get_pointer(StringView self, DA_(StringView) svs) {
  u i = sv_get_index(self, svs);
  if (svs.count!=i) {
    return svs.items + i;
  }
  return NULL;
}
bool sv_is_in(StringView self, DA_(StringView) svs) {
  return svs.count != sv_get_index(self, svs);
}

DA_(StringView) svs_from_strs(str* str_src) {
  DA_(StringView) svs = {0};
  while (*str_src != NULL) {
    da_push(&svs, sv_from_str( *(str_src++) ) );
  }
  return svs;
}
u sv_match_from(DA_(StringView) svs, StringView open_sv, StringView close_sv, u i) {
  u matched_i = i;
  da_bump_start(&svs, matched_i);
  u parity = 0;
  foreach( StringView as sv in svs ) {
    if (sv_starts_with_sv(sv, open_sv)) {sv.items++;break;}
  }
  foreach( StringView as sv in svs ) {
    if (sv_starts_with_sv(sv, open_sv)) parity++;
    if (sv_starts_with_sv(sv, close_sv)) parity--;
    if (parity == 0) break;
    matched_i++;
  }
  return matched_i;
}
u sv_match(DA_(StringView) svs, StringView open_sv, StringView close_sv) {
  return sv_match_from(svs, open_sv, close_sv, 0);
}

#endif //     NOU_STRING_VIEW_IMPLEMENTATION

#endif //   NOU_STRING_VIEW_DYNAMIC_ARRAY_INTERLOP_H_
#endif // DYNAMIC_ARRAY_H_
