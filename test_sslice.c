#include "sslice.h"
#include "stdio.h"

#define RED   "\033[1;31m"
#define RED_N "\033[0;31m"
#define GRN   "\033[1;32m"
#define GRN_N "\033[0;32m"
#define RST   "\033[0m"

#define TEST_PASS(name) \
  fprintf(stdout, GRN "PASSED: %s in %s %s:%i"RST"\n",\
      name, __func__, __FILE__, __LINE__)
#define TEST_FAIL(name) \
  fprintf(stdout, RED "FAILED: %s in %s %s:%i"RST"\n",\
      name, __func__, __FILE__, __LINE__)

#define TEST_FAIL_MSG(name, msg, ...) \
  fprintf(stdout, RED "FAILED: %s in %s %s:%i"RST"\n" msg "\n",\
      name, __func__, __FILE__, __LINE__, __VA_ARGS__)

#define TEST(name, cond) cond; cond ? TEST_PASS(name) : TEST_FAIL(name);

#define TEST_SSLICE_EQ(arg, expected, name) sslice_eq(arg, expected); \
  sslice_eq(arg, expected) ? TEST_PASS(name) : TEST_FAIL_MSG(name, "-> Expected '%.*s' but got '%.*s'", expected.len, expected.start, arg.len, arg.start)

// SSlice trim_len(SSlice to_trim, int amount);
// SSlice chop_delim(SSlice text, char delim);
// SSlice chop_delim_right(SSlice text, char delim);
// SSlice chop_slice(SSlice text, SSlice delim);
// SSlice chop_slice_right(SSlice text, SSlice delim);
// SSlice trim_whitespace_left(SSlice line);
// SSlice chop_line(SSlice text);
// bool begins_with(SSlice slice, SSlice begin);
// bool ends_with(SSlice slice, SSlice end);
// bool sslice_eq(SSlice a, SSlice b);

bool test_trim_len(void) {
  bool suc1 = TEST_SSLICE_EQ(trim_len(SSLICE_NEW("123456"), 3), SSLICE_NEW("456"),
                             "Positive amount");
  bool suc2 = TEST_SSLICE_EQ(trim_len(SSLICE_NEW("123456"), -3), SSLICE_NEW("123"),
                             "Negative amount");
  return suc1 && suc2;
}

bool test_chop_delim(void) {
  SSlice result = chop_delim(SSLICE_NEW("abc def"), ' ');
  bool ret = TEST_SSLICE_EQ(result, SSLICE_NEW("abc"), "Default");
  return ret;
}

bool test_chop_delim_right(void) {
  SSlice result = chop_delim_right(SSLICE_NEW("abc defg"), ' ');
  bool ret = TEST_SSLICE_EQ(result, SSLICE_NEW("defg"), "Default");
  return ret;
}

bool test_chop_slice(void) {
  SSlice result = chop_slice(SSLICE_NEW("test test // comment"), SSLICE_NEW("//"));
  bool ret = TEST_SSLICE_EQ(result, SSLICE_NEW("test test "), "Default");
  return ret;
}

bool test_chop_slice_right(void) {
  SSlice result = chop_slice_right(SSLICE_NEW("test test // comment"), SSLICE_NEW("//"));
  bool ret = TEST_SSLICE_EQ(result, SSLICE_NEW(" comment"), "Default");
  return ret;
}

bool test_trim_whitespace(void) {
  SSlice result = trim_whitespace(SSLICE_NEW("   aaaaaa"));
  bool ret = TEST_SSLICE_EQ(result, SSLICE_NEW("aaaaaa"), "Default");
  return ret;
}

bool test_trim_whitespace_right(void) {
  SSlice result = trim_whitespace_right(SSLICE_NEW("   aaaaaa    "));
  bool ret = TEST_SSLICE_EQ(result, SSLICE_NEW("   aaaaa"), "Default");
  return ret;
}

bool test_chop_line(void) {
  SSlice result = chop_line(SSLICE_NEW("This is a line.\nThis is another.\n"));
  bool ret = TEST_SSLICE_EQ(result, SSLICE_NEW("This is a line."), "Default");
  return ret;
}

bool test_begins_with(void) {
  bool ret1 = TEST("Positive", begins_with(SSLICE_NEW("abcd"), SSLICE_NEW("ab")));
  bool ret2 = TEST("Negative", !begins_with(SSLICE_NEW("abcd"), SSLICE_NEW("balls")));
  return ret1 && ret2;
}

bool test_ends_with(void) {
  bool ret1 = TEST("Positive", ends_with(SSLICE_NEW("abcd"), SSLICE_NEW("cd")));
  bool ret2 = TEST("Negative", !ends_with(SSLICE_NEW("abcd"), SSLICE_NEW("balls")));
  return ret1 && ret2;
}

bool test_sslice_eq(void) {
  bool ret1 = TEST("Equality", sslice_eq(SSLICE_NEW("test"), SSLICE_NEW("test")));
  bool ret2 = TEST("Inequality", !sslice_eq(SSLICE_NEW("test"), SSLICE_NEW("te2st")));
  return ret1 && ret2;
}

typedef bool(*test_func)(void);

int main(void) {
  test_func funcs[] = {
    &test_sslice_eq,
    &test_trim_len,
    &test_chop_delim,
    &test_chop_delim_right,
    &test_chop_slice,
    &test_chop_slice_right,
    &test_trim_whitespace,
    &test_trim_whitespace_right,
    &test_chop_line,
    &test_begins_with,
    &test_ends_with,
  };

  for (int i = 0; i < sizeof(funcs) / sizeof(funcs[0]); i++) {
    if (!funcs[i]()) break;
  }
}
