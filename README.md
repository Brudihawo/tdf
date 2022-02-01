# TODO Finder
Finds todos recursively in a directory

## Quickstart

```commandline
  $ git clone https://github.com/brudihawo/tdf.git
  $ cd tdf
  $ make
```

## In-File TODOs
- [ ] ./sslice.h:19: TODO: Rename Functions
- [ ] ./main.c:53: TODO: handle file types better
- [ ] ./main.c:169: TODO: Extract comment strings to a place where they can be easily modified / appended

## Roadmap
- [ ] Input Custom Comment String
- [ ] Input from stdin
- [ ] Wrap file names in backticks via format option

## Features
- Finds TODO, FIXME, BUG comments and outputs them in a specified format
- Currently, only support for comments starting at the beginning of the line
- Support for a bunch of file types (rust, c / cpp, tex, python, lua).

| Format Option | Description                  |
|---------------|------------------------------|
| `--no-loc`    | Dont print file and location |
| `--plain`     | No markdown list formatting  |
