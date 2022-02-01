#ifndef SSLICE_H
#define SSLICE_H

#include "stdbool.h"
#include "string.h"

typedef struct {
  const char *start;
  int len;
} SSlice;

#define SSLICE_AT(sslice, idx) sslice.start[idx]
#define SSLICE_FP(sslice) sslice.len, sslice.start
#define SSLICE_NEW(content)                                                    \
  (SSlice) { .start = content, .len = strlen(content) }
#define SSLICE_NWL(content, size)                                              \
  { .start = content, .len = size }

SSlice trim_len(SSlice to_trim, int amount);
SSlice chop_delim(SSlice text, char delim);
SSlice chop_delim_right(SSlice text, char delim);
SSlice chop_slice(SSlice text, SSlice delim);
SSlice chop_slice_right(SSlice text, SSlice delim);
SSlice trim_whitespace(SSlice line);
SSlice trim_whitespace_right(SSlice line);
SSlice chop_line(SSlice text);
bool begins_with(SSlice slice, SSlice begin);
bool ends_with(SSlice slice, SSlice end);
bool sslice_eq(SSlice a, SSlice b);
#endif // SSLICE_H
