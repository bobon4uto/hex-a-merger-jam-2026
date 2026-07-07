#ifndef   NOU_TEST_H
#define   NOU_TEST_H
typedef struct sTest {
  cstr message;
  bool passed;
} Test;
Test test_pass(cstr mess);
Test test_fail(cstr mess);
#define TOK test_pass(__func__)
#define TERR test_fail(__func__)

#ifndef nou_log
#include <stdio.h> // for printf
#define nou_log printf
#endif // nou_log
#define do_test(test) {Test t = test(); if (!t.passed) { nou_log("TEST FAIL: %s\n", t.message); return t; } else { nou_log("TEST PASS: %s\n", t.message); } }
#define skip_test(test) { nou_log("TEST SKIPPED: " #test "\n"); }

Test test_all();
#ifdef NOU_STRING_VIEW_H_
Test test_StringView();
 Test test_StringView_sv_from_str();
 Test test_StringView_sv_from_sized_str();
 Test test_StringView_sv_bump_start();
 Test test_StringView_sv_bump_end();
 Test test_StringView_sv_is_equal();
 Test test_StringView_sv_str_at();
 Test test_StringView_sv_char_at();
 Test test_StringView_sv_start();
 Test test_StringView_sv_end();
 Test test_StringView_sv_first();
 Test test_StringView_sv_last();
 Test test_StringView_sv_peekl();
 Test test_StringView_sv_peekr();
 Test test_StringView_sv_chopr();
 Test test_StringView_sv_chopl();
 #ifdef NOU_STRING_BUILDER_H_
 Test test_StringView_sv_from_sb();
 Test test_StringView_sv_write_to_file();
 #endif // NOU_STRING_BUILDER_H_
 #ifdef DYNAMIC_ARRAY_H_
 Test test_StringView_sv_splitl();
 Test test_StringView_sv_splitr();
 #endif //DYNAMIC_ARRAY_H_

#endif // NOU_STRING_VIEW_H_


#ifdef NOU_STRING_BUILDER_H_
  Test test_StringBuilder();

Test test_StringBuilder_sb_is_equal       ();
Test test_StringBuilder_sb_is_exact       ();
Test test_StringBuilder_sb_push_c         ();
Test test_StringBuilder_sb_push_sized_str ();
Test test_StringBuilder_sb_from_sized_str ();
Test test_StringBuilder_sb_push_str       ();
Test test_StringBuilder_sb_from_str       ();
Test test_StringBuilder_sb_push_sv        ();
Test test_StringBuilder_sb_from_sv        ();
Test test_StringBuilder_sb_write_to_file  ();
Test test_StringBuilder_sb_pushfile       ();
Test test_StringBuilder_sb_from_file      ();
Test test_StringBuilder_sb_from_file_safe ();
Test test_StringBuilder_sb_from_file      ();

#endif // NOU_STRING_BUILDER_H_





#endif // NOU_TEST_H
#ifdef    NOU_TEST_IMPLEMENTATION
Test test_all() {
#ifdef NOU_STRING_VIEW_H_
  do_test(test_StringView);
#endif //  NOU_STRING_VIEW_H_
#ifdef NOU_STRING_BUILDER_H_
  do_test(test_StringBuilder);
#endif //  NOU_STRING_BUILDER_H_
  return TOK;
}
Test test_StringView() {
  do_test(test_StringView_sv_from_str);
  do_test(test_StringView_sv_from_sized_str);
  do_test(test_StringView_sv_bump_start);
  do_test(test_StringView_sv_bump_end);
  do_test(test_StringView_sv_is_equal);
  do_test(test_StringView_sv_str_at);
  do_test(test_StringView_sv_char_at);
  do_test(test_StringView_sv_start);
  do_test(test_StringView_sv_end);
  do_test(test_StringView_sv_first);
  do_test(test_StringView_sv_last);
  do_test(test_StringView_sv_peekl);
  do_test(test_StringView_sv_peekr);
  do_test(test_StringView_sv_chopr);
  do_test(test_StringView_sv_chopl);
  #ifdef NOU_STRING_BUILDER_H_
  do_test(test_StringView_sv_from_sb);
  do_test(test_StringView_sv_write_to_file);
  #else
  skip_test(test_StringView_sv_from_sb);
  skip_test(test_StringView_sv_write_to_file);
  #endif //NOU_STRING_BUILDER_H_
  #ifdef DYNAMIC_ARRAY_H_
  do_test(test_StringView_sv_splitl);
  do_test(test_StringView_sv_splitr);
#else
  skip_test(test_StringView_sv_splitl);
  skip_test(test_StringView_sv_splitr);
  #endif // DYNAMIC_ARRAY_H_
  return TOK;
}



Test test_pass(cstr mess) {
  Test t = {0};
  t.message = mess;
  t.passed = true;
  return t;
}
Test test_fail(cstr mess) {
  Test t = {0};
  t.message = mess;
  t.passed = false;
  return t;
}


#ifdef NOU_STRING_VIEW_H_
#define LOG_SV(sv) printf("%.*s", SV_FMT(sv));
Test test_StringView_sv_from_str() {
  StringView sv_from = sv_from_str("hello");
  StringView sv_real = {0};
  sv_real.items = "hello";
  sv_real.count = 5;
  if (sv_is_equal(sv_from, sv_real)) {
    return TOK;
  }
  return TERR;
}
Test test_StringView_sv_from_sized_str() {
  StringView sv_from = sv_from_sized_str("hello, Mario", 5);
  StringView sv_real = {0};
  sv_real.items = "hello";
  sv_real.count = 5;
  if (sv_is_equal(sv_from, sv_real)) {
    return TOK;
  }
  return TERR;

}
Test test_StringView_sv_bump_start() {
  StringView sv_from = sv_from_str("hello, Mario");
  StringView sv_bumped = sv_bump_start(sv_from, 7);
  StringView sv_real = {0};
  sv_real.items = "Mario";
  sv_real.count = 5;
  if (sv_is_equal(sv_bumped , sv_real)) {
  
    return TOK;
  }
  return TERR;
}
Test test_StringView_sv_bump_end() {
  StringView sv_from = sv_from_str("hello, Mario");
  StringView sv_bumped = sv_bump_end(sv_from, 7);
  StringView sv_real = {0};
  sv_real.items = "hello";
  sv_real.count = 5;
  if (sv_is_equal(sv_bumped , sv_real)) {
  
    return TOK;
  }
  return TERR;
}
Test test_StringView_sv_is_equal() {
  StringView sv_real1 = {0};
  sv_real1.items = "hello there";
  sv_real1.count = 5;
  StringView sv_real2 = {0};
  sv_real2.items = "hello";
  sv_real2.count = 5;
  if (sv_is_equal(sv_real1 , sv_real2)) {
    return TOK;
  }
  return TERR;
}
Test test_StringView_sv_str_at() {
  cstr source = "hello";
  StringView sv_from = sv_from_str(source);
  if (
         sv_str_at(sv_from, 2) == source+2
      && sv_str_at(sv_from, 10) == NULL
      ) {
    return TOK;
  }
  return TERR;
}
Test test_StringView_sv_char_at() {
  cstr source = "hello";
  StringView sv_from = sv_from_str(source);
  if (
      sv_char_at(sv_from, 2) == source[2]
      && sv_char_at(sv_from, 10) == '\0'
      ) {
    return TOK;
  }
  return TERR;
}
Test test_StringView_sv_start() {
  cstr source = "hello";
  StringView sv_from = sv_from_str(source);
  if (sv_start(sv_from) == source ) {
    return TOK;
  }
  return TERR;
}
Test test_StringView_sv_end() {
  cstr source = "hello";
  StringView sv_from = sv_from_str(source);
  if (sv_end(sv_from) == source + 4 ) {
    return TOK;
  }
  return TERR;
}
Test test_StringView_sv_first() {
  cstr source = "hello";
  StringView sv_from = sv_from_str(source);
  if (sv_first(sv_from) == source[0] ) {
    return TOK;
  }
  return TERR;
}
Test test_StringView_sv_last() {
  cstr source = "hello";
  StringView sv_from = sv_from_str(source);
  if (sv_last(sv_from) == source[4] ) {
    return TOK;
  }
  return TERR;
}
u _test_cmp_fn_l(StringView sv) {
  if (sv_first(sv) == ' ') {
    return 1;
  } else {
    return 0;
  }
}
u _test_cmp_fn_r(StringView sv) {
  if (sv_last(sv) == ' ') {
    return 1;
  } else {
    return 0;
  }
}
Test test_StringView_sv_peekl() {
  StringView src = sv_from_str("this is a sentance");
  StringView peek = sv_peekl(src, _test_cmp_fn_l);
  if ( sv_is_equal(peek, sv_from_str("this") ) ) {
    return TOK;
  }
  return TERR;
}
Test test_StringView_sv_peekr() {
  StringView src = sv_from_str("this is a sentance");
  StringView peek = sv_peekr(src, _test_cmp_fn_r);
  if ( sv_is_equal(peek, sv_from_str("sentance") ) ) {
    return TOK;
  }
  return TERR;
}
Test test_StringView_sv_chopr() {
  StringView src = sv_from_str("this is a sentance");
  StringView chopped1 = sv_chopr(&src, _test_cmp_fn_r);
  StringView chopped2 = sv_chopr(&src, _test_cmp_fn_r);
  StringView chopped3 = sv_chopr(&src, _test_cmp_fn_r);
  if (
         sv_is_equal(chopped1, sv_from_str("sentance") )
      && sv_is_equal(chopped2, sv_from_str(" ") )
      && sv_is_equal(chopped3, sv_from_str("a") )
      && sv_is_equal(src, sv_from_str("this is ") )
      ) {
    return TOK;
  }
  return TERR;
}
Test test_StringView_sv_chopl() {
  StringView src = sv_from_str("this is a sentance");
  StringView chopped1 = sv_chopl(&src, _test_cmp_fn_l);
  StringView chopped2 = sv_chopl(&src, _test_cmp_fn_l);
  StringView chopped3 = sv_chopl(&src, _test_cmp_fn_l);
  if (
         sv_is_equal(chopped1, sv_from_str("this") )
      && sv_is_equal(chopped2, sv_from_str(" ") )
      && sv_is_equal(chopped3, sv_from_str("is") )
      && sv_is_equal(src, sv_from_str(" a sentance") )
      ) {
    return TOK;
  }
  return TERR;
}

#ifdef NOU_STRING_BUILDER_H_
Test test_StringView_sv_from_sb() {
  StringView sv_from = sv_from_sb( sb_from_str("hello") );
  StringView sv_real = {0};
  sv_real.items = "hello";
  sv_real.count = 5;
  if (sv_is_equal(sv_from , sv_real)) {
    return TOK;
  }
  return TERR;
}
Test test_StringView_sv_write_to_file() {
  StringView sv = sv_from_str("hello");
  Error err = sv_write_to_file(sv, "test.txt");
  if (err == OK) {
    return TOK;
  }
  return TERR;
}
#endif // NOU_STRING_BUILDER_H_

#ifdef DYNAMIC_ARRAY_H_
Test test_StringView_sv_splitl() {
  StringView sv = sv_from_str("00 22 44");
  DA_(StringView) svs = sv_splitl(sv, _test_cmp_fn_l);
  if (
      svs.count == 5
      && sv_is_equal(svs.items[0], sv_from_str("00"))
      && sv_is_equal(svs.items[1], sv_from_str(" "))
      && sv_is_equal(svs.items[2], sv_from_str("22"))
      && sv_is_equal(svs.items[3], sv_from_str(" "))
      && sv_is_equal(svs.items[4], sv_from_str("44"))
      ) {
    return TOK;
  }
  return TERR;
}
Test test_StringView_sv_splitr() {
  StringView sv = sv_from_str("44 22 00");
  DA_(StringView) svs = sv_splitr(sv, _test_cmp_fn_r);
  if (
      svs.count == 5
      && sv_is_equal(svs.items[0], sv_from_str("00"))
      && sv_is_equal(svs.items[1], sv_from_str(" "))
      && sv_is_equal(svs.items[2], sv_from_str("22"))
      && sv_is_equal(svs.items[3], sv_from_str(" "))
      && sv_is_equal(svs.items[4], sv_from_str("44"))
      ) {
    return TOK;
  }
  return TERR;
}
#endif //DYNAMIC_ARRAY_H_
#endif // NOU_STRING_VIEW_H_


#ifdef NOU_STRING_BUILDER_H_
Test test_StringBuilder_sb_is_equal       () {
  StringBuilder sb_from = sb_from_str("hello");
  str src = "hello. hi.";
  StringBuilder sb_real = {0};
  sb_real.items = src;
  sb_real.count = 5;
  if (
      sb_is_equal(sb_from, sb_real)
      ) {
    return TOK;
  }
  return TERR;
}
Test test_StringBuilder_sb_is_exact       () {
  StringBuilder sb_from = sb_from_str("hello");
  str src = "hello. hi.";
  StringBuilder sb_real = {0};
  sb_real.items = src;
  sb_real.count = 5;
  if (
      !sb_is_exact(sb_from, sb_real)
      && sb_is_exact(sb_from, sb_from)
      ) {
    return TOK;
  }
  return TERR;
}
Test test_StringBuilder_sb_push_c         () {
  return TOK;
}
Test test_StringBuilder_sb_push_sized_str () {
  StringBuilder sb_from = sb_from_str("hello");
  sb_push_sized_str(&sb_from, "hi there", 2);
  str src = "hellohi";
  StringBuilder sb_real = {0};
  sb_real.items = src;
  sb_real.count = 7;
  if (
      sb_is_equal(sb_from, sb_real)
      ) {
    return TOK;
  }
  return TERR;
}
Test test_StringBuilder_sb_from_sized_str (){
  StringBuilder sb_from = sb_from_sized_str("hello there", 5);
  sb_push_str(&sb_from, "hi" );
  str src = "hellohi";
  StringBuilder sb_real = {0};
  sb_real.items = src;
  sb_real.count = 7;
  if (
      sb_is_equal(sb_from, sb_real)
      ) {
    return TOK;
  }
  return TERR;
}
Test test_StringBuilder_sb_push_str       (){
  StringBuilder sb_from = sb_from_str("hello");
  sb_push_str(&sb_from, "hi" );
  str src = "hellohiho";
  StringBuilder sb_real = {0};
  sb_real.items = src;
  sb_real.count = 7;
  StringBuilder sb_real2 = {0};
  sb_real2.items = src;
  sb_real2.count = 9;
  if (
      sb_is_equal(sb_from, sb_real)
      && sb_is_equal(sb_push_str(&sb_from, "ho"), sb_real2)
      ) {
    return TOK;
  }
  return TERR;
}
Test test_StringBuilder_sb_from_str       (){
  StringBuilder sb_from = sb_from_str("hello");
  str src = "hello";
  StringBuilder sb_real = {0};
  sb_real.items = src;
  sb_real.count = 5;
  if (
      sb_is_equal(sb_from, sb_real)
      ) {
    return TOK;
  }
  return TERR;
}
Test test_StringBuilder_sb_push_sv        (){
  StringBuilder sb_from = sb_from_str("hello");
  str src = "hellohi";
  sb_push_sv(&sb_from, sv_from_str("hi"));
  StringBuilder sb_real = {0};
  sb_real.items = src;
  sb_real.count = 7;
  if (
      sb_is_equal(sb_from, sb_real)
      ) {
    return TOK;
  }
  return TERR;
}
Test test_StringBuilder_sb_from_sv        (){
  StringBuilder sb_from = sb_from_sv(sv_from_str("hello"));
  str src = "hellohi";
  sb_push_sv(&sb_from, sv_from_str("hi"));
  StringBuilder sb_real = {0};
  sb_real.items = src;
  sb_real.count = 7;
  if (
      sb_is_equal(sb_from, sb_real)
      ) {
    return TOK;
  }
  return TERR;
}
Test test_StringBuilder_sb_write_to_file  () {
  StringBuilder sb = sb_from_str("hi here");
  Error err = sb_write_to_file(sb,"test.txt");
  if (err==OK) {
    return TOK;
  }
  
  return TERR;
}
Test test_StringBuilder_sb_pushfile() {
  StringBuilder sb = sb_from_str("hi here");
  Error err = sb_write_to_file(sb,"test.txt");
  if (err==OK) {
    return TOK;
  }
  StringBuilder sb2 = {0};
  sb_pushfile(&sb2, "test.txt");
  if (
      sb_is_equal(sb, sb2)
      ) {
    return TOK;
  }
  return TERR;
}
Test test_StringBuilder_sb_from_file      (){
  StringBuilder sb = sb_from_str("hi here");
  Error err = sb_write_to_file(sb,"test.txt");
  if (err==OK) {
    return TOK;
  }
  StringBuilder sb2 = sb_from_file("test.txt");
  if (
      sb_is_equal(sb, sb2)
      ) {
    return TOK;
  }
  return TERR;
}
Test test_StringBuilder_sb_from_file_safe (){

  StringBuilder sb = sb_from_str("hi here");
  Error err = sb_write_to_file(sb,"test.txt");
  if (err==OK) {
    return TOK;
  }
  ResOfStringBuilder sb2res = sb_from_file_safe("test.txt");
  if (sb2res.err !=OK) {
    return TERR;
  }
  if (
      !sb_is_equal(sb, sb2res.ok)
      ) {
    return TERR;
  }
  ResOfStringBuilder sb3res = sb_from_file_safe("test.txt.nonexistant");
  if (sb3res.err !=OK) {
    return TOK;
  }

  return TERR;
}


Test test_StringBuilder() {
  do_test( test_StringBuilder_sb_is_equal       );
  do_test( test_StringBuilder_sb_is_exact       );
  do_test( test_StringBuilder_sb_push_c         );
  do_test( test_StringBuilder_sb_push_sized_str );
  do_test( test_StringBuilder_sb_from_sized_str );
  do_test( test_StringBuilder_sb_push_str       );
  do_test( test_StringBuilder_sb_from_str       );
  do_test( test_StringBuilder_sb_push_sv        );
  do_test( test_StringBuilder_sb_from_sv        );
  do_test( test_StringBuilder_sb_write_to_file  );
  do_test( test_StringBuilder_sb_pushfile       );
  do_test( test_StringBuilder_sb_from_file      );
  do_test( test_StringBuilder_sb_from_file_safe );
  return TOK;
}


#endif // NOU_STRING_BUILDER_H_



#endif // NOU_TEST_IMPLEMENTATION
