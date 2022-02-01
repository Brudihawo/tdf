#include "assert.h"
#include "errno.h"
#include "ftw.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "sslice.h"

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

// default arguments
bool file_loc = true;      // print file and location of todo
bool markdown_list = true; // print list in markdown format

typedef enum {
  PLAIN = 0,
  MARKDOWN_LIST_PLAIN,
  FILE_LOC,
  MARKDOWN_LIST_FULL,
} OutputFormat;

OutputFormat output_fmt;

bool is_comment(SSlice line) {
  SSlice trimmed = trim_whitespace(line);
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

typedef struct {
  FILE* file;
  size_t size;
} FPackage;

FPackage open_safe(const char* fname) {
  FILE *f = fopen(fname, "r");
  if (f == NULL) {
    fprintf(stderr, "Could not open file %s: %s\n", fname, strerror(errno));
    exit(1);
  }
  if (fseek(f, 0, SEEK_END) < 0) {
    fprintf(stderr, "Error getting file size for %s: %s\n", fname,
            strerror(errno));
    exit(1);
  }
  size_t f_size = ftell(f);
  if (fseek(f, 0, SEEK_SET) < 0) {
    fprintf(stderr, "Error setting position in file %s: %s\n", fname,
            strerror(errno));
    exit(1);
  }
  return (FPackage) {
    .file = f,
    .size = f_size,
  };
}

void process_file(const char *fname) {

  FPackage f = open_safe(fname);
  file_type = get_filetype(fname);

  size_t cur_pos = 0;
  size_t line_no = 0;
  while (cur_pos < f.size) {
    // reset file and file buffer
    fseek(f.file, cur_pos, SEEK_SET);
    memset(&strbuf, 0, STRBUF_CAP);

    // read file chunk into string buffer
    fread(&strbuf, STRBUF_CAP, 1, f.file);
    SSlice cur_chunk = (SSlice){
        .start = strbuf,
        .len = STRBUF_CAP,
    };
    while (true) {
      SSlice cur_line = chop_line(cur_chunk);
      line_no++;
      cur_pos += cur_line.len + 1;
      if (cur_line.len > 0) {
        // TODO: support end of line comments as well
        if (is_comment(cur_line)) {
          // trim comment characters and 'TODO: '
          SSlice trimmed = trim_whitespace(cur_line);
          trimmed = trim_len(trimmed, comstrs[file_type].len);
          trimmed = trim_whitespace(trimmed);
          // TODO: Extract comment strings to a place where they can be easily modified / appended
          if (begins_with(trimmed, SSLICE_NEW("TODO: ")) ||
              begins_with(trimmed, SSLICE_NEW("FIXME: ")) ||
              begins_with(trimmed, SSLICE_NEW("BUG: "))) {
            switch (output_fmt) {
            case PLAIN:
              fprintf(stdout, "%.*s\n", SSLICE_FP(trimmed));
              break;
            case FILE_LOC:
              fprintf(stdout, "%s:%ld: %.*s\n", fname, line_no,
                      SSLICE_FP(trimmed));
              break;
            case MARKDOWN_LIST_PLAIN:
              fprintf(stdout, "- [ ] %.*s\n", SSLICE_FP(trimmed));
              break;
            case MARKDOWN_LIST_FULL:
              fprintf(stdout, "- [ ] %s:%ld: %.*s\n", fname, line_no,
                      SSLICE_FP(trimmed));
              break;
            }
          }
        }
      } else if (cur_line.len == -1) {
        cur_pos = f.size;
        break;
      } else {
        break;
      }
      cur_chunk = trim_len(cur_chunk, cur_line.len + 1);
    }
  }

  fclose(f.file);
}

int ftw_process_path(const char *fpath, const struct stat *sb, int typeflag) {
  if (typeflag == FTW_F) {
    process_file(fpath);
  }
  return 0;
}

void usage() {
  fprintf(stderr, "Usage: tdf [options] [path]\n");
  fprintf(stderr, "Finds TODOs in folder recursively and outputs "
                  "them in markdown-ready format\n");
  fprintf(stderr, "If no path is provided, will traverse the current "
                  "folder and subdirectories.\n");
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  --help, -h     Show this help\n");
  fprintf(stderr, "  --no-loc       Do not print file and location\n");
  fprintf(stderr, "  --plain        Omit markdown list formatting\n");
  exit(1);
}

int main(int argc, char **argv) {
  for (int i = 1; i < argc; i++) { // process optional arguments
    SSlice cur_arg = SSLICE_NEW(argv[i]);
    if (sslice_eq(cur_arg, SSLICE_NEW("-h")) ||
        sslice_eq(cur_arg, SSLICE_NEW("--help"))) {
      usage();
    }
  }

  char *file_path;
  size_t optional_args_count = 0;
  for (int i = 1; i < argc; i++) { // process optional arguments
    SSlice cur_arg = SSLICE_NEW(argv[i]);
    if (sslice_eq(cur_arg, SSLICE_NEW("--plain"))) {
      optional_args_count++;
      markdown_list = false;
    } else if (sslice_eq(cur_arg, SSLICE_NEW("--no-loc"))) {
      optional_args_count++;
      file_loc = false;
    } else {
      file_path = argv[i];
      if (i != argc - 1) {
        fprintf(stderr, "Invalid arguments\n");
        usage();
      }
      break;
    }
  }

  // set output format
  if (markdown_list && file_loc) {
    output_fmt = MARKDOWN_LIST_FULL;
  } else if (markdown_list && !file_loc) {
    output_fmt = MARKDOWN_LIST_PLAIN;
  } else if (!markdown_list && file_loc) {
    output_fmt = FILE_LOC;
  } else {
    output_fmt = PLAIN;
  }

  if (argc > optional_args_count + 2) {
    usage();
  }

  if (argc == optional_args_count + 2) {
    ftw(file_path, ftw_process_path, 10);
  } else if (argc == optional_args_count + 1) {
    ftw(".", ftw_process_path, 10);
  }

  return 0;
}
