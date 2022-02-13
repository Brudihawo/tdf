/**
 * @file sl.h
 * @author Hawo Höfer
 * @version 1.0
 *
 * @section License
 * Copyright 2022 Hawo Höfer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef SL_H
#define SL_H

#include "stdbool.h"
#include "string.h"

/**
 * String Slice object
 */
typedef struct {
  const char *start; /**< @param start Pointer to character array */
  int len;           /**< length of sl */
} SL;

/** printf format string for SL (use together with SL_FP)*/
#define SL_FMT "%s.*"

/** printf SL processing for SL (use together with SL_FMT)*/
#define SL_FP(sslice) sslice.len, sslice.start

/** Access SL at index */
#define SL_AT(sl, idx) sl.start[idx]

/** New SL from content */
#define SL_NEW(content)                                                    \
  (SL) { .start = content, .len = strlen(content) }

/** New SL from Content and size for constant definition */
#define SL_NWL(content, size)                                              \
  { .start = content, .len = size }

/**
 * @brief Trim chars from an SL.
 *
 * If amount is negative, trim from the right. Otherwise,
 * trim from the left.
 *
 * @param to_trim SL to trim
 * @param amount number of characters to remove
 *
 * @return Trimmed SL
 */
SL SL_trim_len(SL to_trim, int amount);

/**
 * @brief Chop off part of an SL by a char delimiter from the left.
 *
 * This will return an SL excluding the delimiter.
 *
 * @param to_chop SL to chop
 * @param delim delimiter character
 *
 * @return Chopped part of SL (beginning until character delimiter)
 */
SL SL_chop_delim(SL to_chop, char delim);

/**
 * @brief Chop off part of an SL by a char delimiter from the right.
 *
 * This will return an SL excluding the delimiter.
 *
 * @param to_chop SL to chop
 * @param delim delimiter character
 *
 * @return Chopped part of SL (character delimiter until end)
 */
SL SL_chop_delim_right(SL to_chop, char delim);

/**
 * @brief Chop off part of an SL by an SL delimiter from the right.
 *
 * This will return an SL excluding the delimiter.
 *
 * @param to_chop SL to chop
 * @param delim delimiter sequence
 *
 * @return Chopped part of SL (beginning until character delimiter)
 */
SL SL_chop_slice(SL to_chop, SL delim);

/**
 * @brief Chop off part of an SL by an SL delimiter from the right.
 *
 * This will return an SL excluding the delimiter.
 *
 * @param to_chop SL to chop
 * @param delim delimiter sequence
 *
 * @return Chopped part of SL (character delimiter until end)
 */
SL SL_chop_slice_right(SL to_chop, SL delim);

/**
 * @brief Trim whitespace from the left of an SL
 *
 * @param to_trim SL to trim
 *
 * @return Trimmed SL
 */
SL SL_trim_whitespace(SL to_trim);

/**
 * @brief Trim whitespace from the right of an SL
 *
 * @param to_trim SL to trim
 *
 * @return Trimmed SL
 */
SL SL_trim_whitespace_right(SL to_trim);

/**
 * @brief Chop a line from an SL
 *
 * Shorthand for SL_chop_delim(to_chop, '\n').
 *
 * @param to_chop SL to chop
 *
 * @return First line in SL.
 */
SL SL_chop_line(SL to_chop);

/**
 * Test start of SL
 *
 * @param to_test SL to test
 * @param begin SL that it begins with
 *
 * @return bool
 */
bool SL_begins_with(SL to_test, SL begin);

/**
 * Test end of SL
 *
 * @param to_test SL to test
 * @param end SL that it ends with
 *
 * @return bool
 */
bool SL_ends_with(SL to_test, SL end);

/**
 * Test equality of SL
 *
 * @param a SL to test
 * @param b SL that it ends with
 *
 * @return bool
 */
bool SL_eq(SL a, SL b);

#ifdef SL_IMPLEMENTATION // INCLUDE IMPLEMENTATIONS
#include "stdio.h"
#include "stdlib.h"

SL SL_trim_len(SL to_trim, int amount) {
  if (amount > 0) {
    return (SL){&SL_AT(to_trim, amount), to_trim.len - amount};
  } else {
    return (SL){&SL_AT(to_trim, 0), to_trim.len + amount};
  }
}

SL SL_chop_delim(SL text, char delim) {
  int i = 0;
  while ((i < text.len) && (SL_AT(text, i) != delim)) {
    i++;
  }
  if (i == text.len) i = -1;

  return (SL){
      .start = &SL_AT(text, 0),
      .len = i,
  };
}

SL SL_chop_delim_right(SL text, char delim) {
  int i = text.len - 1;
  while ((i >= 0) && (SL_AT(text, i) != delim)) {
    i--;
  }
  if (i == text.len) i = -1;

  return (SL) {
      .start = &SL_AT(text, i + 1),
      .len = i == -1 ? i : text.len - i - 1,
  };
}

SL SL_chop_slice(SL to_chop, SL delim) {
  int i = 0;
  int j = 0;
  while (i < to_chop.len && j < delim.len) {
    // This loop breaks if the number of matching chars is delim.len
    if (SL_AT(to_chop, i) == SL_AT(delim, j)) {
      j++;
      i++;
    } else {
      if (j == 0) i++;
      else j = 0;
    }
  }

  return (SL) {
    .start = &SL_AT(to_chop, 0),
    .len = j == 0 ? -1 : i - delim.len,
  };
}

SL SL_chop_slice_right(SL to_chop, SL delim) {
  int i = to_chop.len - 1;
  int j = delim.len - 1;
  while (i >= 0 && j >= 0) {
    // This loop breaks if the number of matching chars is delim.len
    if (SL_AT(to_chop, i) == SL_AT(delim, j)) {
      j--;
      i--;
    } else {
      if (j == delim.len - 1) i--;
      else j = delim.len - 1;
    }
  }

  return (SL) {
    .start = &SL_AT(to_chop, i + delim.len + 1),
    .len = j == 0 ? -1 : to_chop.len - i - delim.len - 1,
  };
}

SL SL_chop_line(SL to_chop) { return SL_chop_delim(to_chop, '\n'); }

SL SL_trim_whitespace(SL to_trim) {
  size_t cur_idx = 0;
  while (SL_AT(to_trim, cur_idx) == ' ') {
    cur_idx++;
  }

  return (SL){
      .start = &SL_AT(to_trim, cur_idx),
      .len = to_trim.len - cur_idx,
  };
}

SL SL_trim_whitespace_right(SL to_trim) {
  size_t cur_idx = 1;
  while (SL_AT(to_trim, to_trim.len - cur_idx) == ' ') {
    cur_idx++;
  }

  return (SL){
      .start = &SL_AT(to_trim, 0),
      .len = to_trim.len - cur_idx + 1,
  };
}

bool SL_begins_with(SL slice, SL begin) {
  if (slice.len < begin.len) return false;

  for (int idx = 0; idx < begin.len; idx++) {
    if (SL_AT(slice, idx) != SL_AT(begin, idx)) {
      return false;
    }
  }
  return true;
}

bool SL_ends_with(SL slice, SL end) {
  if (slice.len < end.len) return false;

  for (int idx = 0; idx < end.len; idx++) {
    if (SL_AT(slice, slice.len - end.len + idx) != SL_AT(end, idx)) {
      return false;
    }
  }
  return true;
}

bool SL_eq(SL a, SL b) {
  if (a.len != b.len) return false;

  for (int i = 0; i < a.len; i++) {
    if (a.start[i] != b.start[i])
      return false;
  }
  return true;
}

#endif // SL_IMPLEMENTATION

#endif // SL_H
