#include "assert.h"
#include "errno.h"
#include "ftw.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
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

SSlice trim_len(SSlice to_trim, int amount) {
  int trim_loc, trimmed_len;
  if (amount > 0) {
    trim_loc = amount;
    trimmed_len = to_trim.len - amount;
  } else {
    trim_loc = to_trim.len - amount;
    trimmed_len = amount;
  }

  return (SSlice){&SSLICE_AT(to_trim, trim_loc), trimmed_len};
}

SSlice chop_delim(SSlice text, char delim) {
  int i = 0;
  while ((i < text.len) && (SSLICE_AT(text, i) != delim)) {
    i++;
  }

  return (SSlice){
      .start = &SSLICE_AT(text, 0),
      .len = i == text.len - 1 ? -1 : i,
  };
}

SSlice chop_delim_right(SSlice text, char delim) {
  int i = text.len - 1;
  while ((i > 0) && (SSLICE_AT(text, i) != delim)) {
    i--;
  }
  if (i == text.len) {
    i = -1;
  }
  return (SSlice){
      .start = &SSLICE_AT(text, i + 1),
      .len = text.len - i + 1,
  };
}

static inline SSlice chop_line(SSlice text) { return chop_delim(text, '\n'); }

SSlice trim_whitespace_left(SSlice line) {
  size_t cur_idx = 0;
  while (SSLICE_AT(line, cur_idx) == ' ') {
    cur_idx++;
  }
  return (SSlice){
      .start = &SSLICE_AT(line, cur_idx),
      .len = line.len - cur_idx,
  };
}

bool begins_with(SSlice slice, SSlice begin) {
  if (slice.len < begin.len)
    return false;

  for (int idx = 0; idx < begin.len; idx++) {
    if (SSLICE_AT(slice, idx) != SSLICE_AT(begin, idx)) {
      return false;
    }
  }
  return true;
}

bool ends_with(SSlice slice, SSlice end) {
  if (slice.len < end.len)
    return false;
  for (int idx = 0; idx < end.len; idx++) {
    if (SSLICE_AT(slice, slice.len - end.len + idx) != SSLICE_AT(end, idx)) {
      return false;
    }
  }
  return true;
}

typedef enum {
  FT_UNKNOWN = -1,
  FT_TXT = 0,
  FT_PYTHON,
  FT_RUST,
  FT_CPP,
  FT_HPP,
  FT_C,
  FT_H,
  FT_LUA,
  FT_TEX,
  FT_N,
} Ftype;

const char *ftype_to_cstr(Ftype ftype) {
  switch (ftype) {
  case FT_UNKNOWN:
    return "UNKNOWN";
  case FT_TXT:
    return "TXT";
  case FT_PYTHON:
    return "PYTHON";
  case FT_RUST:
    return "RUST";
  case FT_CPP:
    return "CPP";
  case FT_HPP:
    return "HPP";
  case FT_C:
    return "C";
  case FT_H:
    return "H";
  case FT_LUA:
    return "LUA";
  case FT_TEX:
    return "TEX";
  case FT_N:
    return "FT_N";
  }
  assert(false && "unreachable");
}

// TODO: handle file types better
SSlice comstrs[FT_N] = {
    [FT_TXT] = SSLICE_NWL("", 0),    [FT_PYTHON] = SSLICE_NWL("#", 1),
    [FT_RUST] = SSLICE_NWL("//", 2), [FT_CPP] = SSLICE_NWL("//", 2),
    [FT_HPP] = SSLICE_NWL("//", 2),  [FT_C] = SSLICE_NWL("//", 2),
    [FT_H] = SSLICE_NWL("//", 2),    [FT_LUA] = SSLICE_NWL("--", 2),
    [FT_TEX] = SSLICE_NWL("%", 1),
};

SSlice endings[FT_N] = {
    [FT_TXT] = SSLICE_NWL(".txt", 4), [FT_PYTHON] = SSLICE_NWL(".py", 3),
    [FT_RUST] = SSLICE_NWL(".rs", 3), [FT_CPP] = SSLICE_NWL(".cpp", 4),
    [FT_HPP] = SSLICE_NWL(".hpp", 4), [FT_C] = SSLICE_NWL(".c", 2),
    [FT_H] = SSLICE_NWL(".h", 2),     [FT_LUA] = SSLICE_NWL(".lua", 4),
    [FT_TEX] = SSLICE_NWL(".tex", 4),
};

Ftype get_filetype(const char *fname) {
  SSlice fname_s = SSLICE_NEW(fname);
  for (int cur_ftype = FT_N - 1; cur_ftype >= FT_UNKNOWN; cur_ftype--) {
    if (ends_with(fname_s, endings[cur_ftype]))
      return cur_ftype;
  }
  return FT_UNKNOWN;
}

#define STRBUF_CAP (8 * 1024)
char strbuf[STRBUF_CAP] = {0};
Ftype file_type = FT_UNKNOWN;

bool is_comment(SSlice line) {
  SSlice trimmed = trim_whitespace_left(line);
  switch (file_type) {
  case FT_N:
    assert(false && "unreachable");
    break;
  case FT_UNKNOWN:
    file_type = FT_TXT;
    break;
  default:
    break;
  }
  return begins_with(trimmed, comstrs[file_type]);
}

void process_file(const char *fname) {
  FILE *f = fopen(fname, "r");
  if (f == NULL) {
    fprintf(stderr, "Could not open file %s: %s\n", fname, strerror(errno));
    exit(1);
  }
  // Get file size
  // TODO: Does this need error handling?
  fseek(f, 0, SEEK_END);
  size_t f_size = ftell(f);
  fseek(f, 0, SEEK_SET);

  file_type = get_filetype(fname);

  size_t cur_pos = 0;
  size_t line_no = 0;
  while (cur_pos < f_size) {
    // reset file and file buffer
    fseek(f, cur_pos, SEEK_SET);
    memset(&strbuf, 0, STRBUF_CAP);

    // read file chunk into string buffer
    fread(&strbuf, STRBUF_CAP, 1, f);
    SSlice cur_chunk = (SSlice){
        .start = strbuf,
        .len = STRBUF_CAP,
    };
    while (true) {
      SSlice cur_line = chop_line(cur_chunk);
      line_no++;
      cur_pos += cur_line.len + 1;
      if (cur_line.len > 0) {
        if (is_comment(cur_line)) {
          // trim comment characters and 'TODO: '
          SSlice trimmed = trim_whitespace_left(cur_line);
          trimmed = trim_len(trimmed, comstrs[file_type].len);
          trimmed = trim_whitespace_left(trimmed);
          if (begins_with(trimmed, SSLICE_NEW("TODO: "))) {
            fprintf(stdout, "- [ ] %s:%ld - %.*s\n", fname, line_no,
                    SSLICE_FP(trimmed));
          }
        }
      } else if (cur_line.len == -1) {
        cur_pos = f_size;
        break;
      } else {
        break;
      }
      cur_chunk = trim_len(cur_chunk, cur_line.len + 1);
    }
  }

  fclose(f);
}

int ftw_process_path(const char *fpath, const struct stat *sb, int typeflag) {
  if (typeflag == FTW_F) {
    process_file(fpath);
  }
  return 0;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: tdf [path]\n");
    exit(1);
  }
  char *dirname = argv[1];
  ftw(dirname, ftw_process_path, 10);

  return 0;
}
