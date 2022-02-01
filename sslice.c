#include "sslice.h"

#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

SSlice trim_len(SSlice to_trim, int amount) {
  if (amount > 0) {
    return (SSlice){&SSLICE_AT(to_trim, amount), to_trim.len - amount};
  } else {
    return (SSlice){&SSLICE_AT(to_trim, 0), to_trim.len + amount};
  }
}

SSlice chop_delim(SSlice text, char delim) {
  int i = 0;
  while ((i < text.len) && (SSLICE_AT(text, i) != delim)) {
    i++;
  }
  if (i == text.len) i = -1;

  return (SSlice){
      .start = &SSLICE_AT(text, 0),
      .len = i,
  };
}

SSlice chop_delim_right(SSlice text, char delim) {
  int i = text.len - 1;
  while ((i >= 0) && (SSLICE_AT(text, i) != delim)) {
    i--;
  }
  if (i == text.len) i = -1; 

  return (SSlice) {
      .start = &SSLICE_AT(text, i + 1),
      .len = i == -1 ? i : text.len - i - 1,
  };
}

SSlice chop_slice(SSlice text, SSlice delim) {
  int i = 0;
  int j = 0;
  while (i < text.len && j < delim.len) {
    // This loop breaks if the number of matching chars is delim.len
    if (SSLICE_AT(text, i) == SSLICE_AT(delim, j)) {
      j++;
      i++;
    } else {
      if (j == 0) i++;
      else j = 0;
    }
  }

  return (SSlice) {
    .start = &SSLICE_AT(text, 0),
    .len = j == 0 ? -1 : i - delim.len,
  };
}

SSlice chop_slice_right(SSlice text, SSlice delim) {
  int i = text.len - 1;
  int j = delim.len - 1;
  while (i >= 0 && j >= 0) {
    // This loop breaks if the number of matching chars is delim.len
    if (SSLICE_AT(text, i) == SSLICE_AT(delim, j)) {
      j--;
      i--;
    } else {
      if (j == delim.len - 1) i--;
      else j = delim.len - 1;
    }
  }

  return (SSlice) {
    .start = &SSLICE_AT(text, i + delim.len + 1),
    .len = j == 0 ? -1 : text.len - i - delim.len - 1,
  };
}

SSlice chop_line(SSlice text) { return chop_delim(text, '\n'); }

SSlice trim_whitespace(SSlice line) {
  size_t cur_idx = 0;
  while (SSLICE_AT(line, cur_idx) == ' ') {
    cur_idx++;
  }

  return (SSlice){
      .start = &SSLICE_AT(line, cur_idx),
      .len = line.len - cur_idx,
  };
}

SSlice trim_whitespace_right(SSlice line) {
  size_t cur_idx = 1;
  while (SSLICE_AT(line, line.len - cur_idx) == ' ') {
    cur_idx++;
  }

  return (SSlice){
      .start = &SSLICE_AT(line, 0),
      .len = line.len - cur_idx + 1,
  };
}

bool begins_with(SSlice slice, SSlice begin) {
  if (slice.len < begin.len) return false;

  for (int idx = 0; idx < begin.len; idx++) {
    if (SSLICE_AT(slice, idx) != SSLICE_AT(begin, idx)) {
      return false;
    }
  }
  return true;
}

bool ends_with(SSlice slice, SSlice end) {
  if (slice.len < end.len) return false;

  for (int idx = 0; idx < end.len; idx++) {
    if (SSLICE_AT(slice, slice.len - end.len + idx) != SSLICE_AT(end, idx)) {
      return false;
    }
  }
  return true;
}

bool sslice_eq(SSlice a, SSlice b) {
  if (a.len != b.len) return false;

  for (int i = 0; i < a.len; i++) {
    if (a.start[i] != b.start[i])
      return false;
  }
  return true;
}
