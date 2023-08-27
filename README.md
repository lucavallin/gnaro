# <img src="./docs/gnaro.png" style="width:40px;padding-right:10px;margin-bottom:-8px;"> gnaro
A proto-database inspired by SQLite, written in C for educational purposes. `gnaro` takes SQLite as a reference because of the limited feature set, and therefore complexity, when compared to other databases. SQLite consists roughly of the following components:

- Tokenizer
- Parser
- Code Generator
- Virtual Machine
- B-Tree
- Pager
- OS Interface

Tokenizer, Parser and Code Generator are part of the front-end, which takes in a SQL query and returns bytecode for the SQL Virtual Machine. A separate front-end reduces the complexity of each component (e.g. virtual machine does not worry about syntax errors) and allows compiling common queries once and caching the bytecode for improved performance. The Virtual Machine executes the bytecode and interacts with the B-Tree, which is responsible for storing and retrieving data. The Pager is responsible for reading and writing pages to and from disk. The OS Interface is responsible for interacting with the operating system.

## Usage

`gnaro` can be run using `gnaro.db` as database file as follows (with the optional `-v` for verbose output):

```bash
$ ./bin/gnaro -d gnaro.db [-v]

gnaro> insert 1 example example@example.com
16:39:33 INFO  ./src/gnaro.c:123: statement executed

gnaro> select
(1, example, example@example.com)
16:39:36 INFO  ./src/gnaro.c:123: statement executed

gnaro> .btree
Tree:
- leaf (size 1)
  - 1

gnaro> .exit
16:39:43 INFO  ./src/gnaro.c:139: freeing resources...
16:39:43 INFO  ./src/gnaro.c:140: freeing input buffer...
16:39:43 INFO  ./src/gnaro.c:142: freeing database...
16:39:43 INFO  ./src/gnaro.c:147: so long and thanks for all the wasps!
```

## Setup

`gnaro` requires a number of tools and libraries to be installed to build the project and for development.

```bash
# Install all required tooling and dependencies
$ sudo apt install -y make
$ make setup
```

### Dependencies

`gnaro` depends on the following "non-standard" libraries:

- `libcuni1`: used for testing with CUnit
- [argtable](http://argtable.org/): used to parse command line arguments
- [rxi/log.c](https://github.com/rxi/log.c): used for logging

`gnaro` uses a number of LLVM-18-based tools for development, linting, formatting, debugging and Valgrind to check for memory leaks.

## Build

The included `Makefile` provides a few targets to build `gnaro`.
The variable `debug=1` can be set to run any of the targets in "debug" mode, which builds the project with debug symbols and without optimizations (especially useful for the debugger and valgrind).

```bash
# Build gnaro (executable is in bin/)
# The default target also runs, "make lint" and "make format" to lint and format the code
$ make


# Build gnaro with debug flags
$ make debug=1
```

## Development
`gnaro` is developed using [Visual Studio Code](https://code.visualstudio.com/) and [GitHub Codespaces](https://github.com/codespaces). The repository contains all the necessary configuration files to use these tools effectively.
`gnaro` relies on low-level Linux features, so it must be run on a Linux system. [GitHub Codespaces](https://github.com/codespaces) acts weird at times when tweaking low-level container settings: I found [getutm.app](https://getutm.app) to work well with [Debian](http://debian.org) on my Mac when in doubt.

The included `Makefile` provides a few targets useful for development:

```bash
# Run tests
$ make test

# Run linter
$ make lint

# Run formatter
$ make format

# Run valgrind
$ make check

# Clean the build
$ make clean

# Generate compilation database for Clang tools
$ make bear
```

Furthermore, the project includes a [Visual Studio Code](https://code.visualstudio.com/) configuration in `.vscode/` that can be used to run the built-in debugger.

## Structure

The project is structured as follows:

```txt
├── .devcontainer           configuration for GitHub Codespaces
├── .github                 configuration GitHub Actions and other GitHub features
├── .vscode                 configuration for Visual Studio Code
├── bin                     the executable (created by make)
├── build                   intermediate build files e.g. *.o (created by make)
├── docs                    documentation
├── include                 header files
├── lib                     third-party libraries
├── scripts                 scripts for setup and other tasks
├── src                     C source files
│   ├── gnaro.c             (main) Entry point for the CLI
│   └── *.c
├── tests                   contains tests
├── .clang-format           configuration for the formatter
├── .clang-tidy             configuration for the linter
├── .gitignore
├── compile_commands.json   compilation database for clang tools
├── gnaro.db                Database file for gnaro
├── LICENSE
├── Makefile
└── README.md
```

## Testing and documentation

At the moment, the project does not contain any automated tests but `Cunit` is set up and a basic test can be found at [gnaro_test.c](tests/gnaro_test.c). Documentation is currently limited to this README and the comments and logging in the code.

## Limitations

`gnaro` is an extremely primitive "database" and all it supports is:

 1. A single database
 1. Rows with hardcoded columns
 1. "insert" and "select" statements

It would be nice to at least support multiple tables, non-harcoded columns, "update" and "delete" statements. Unfortunately the goal of this project is only to learn more about databases and I am reasonably satisfied with the current state.

TODO: Furthermore, exits here and there, dependency between "modules".

## Improvements

- CMake and Conan are industry standards, so they should be used eventually instead of Make and the current build system. Unfortunately, CMake and Conan also add a lot of complexity which is not needed at this time.

## Credits

`gnaro` is based almost entirely on [db_tutorial](https://cstack.github.io/db_tutorial/) by [@cstack](https://github.com/cstack).

## FAQ

- **Why C?** I haven't written much C since college and nostalgia got me.
- **What does "gnaro" mean?** It's [Venetian](https://vec.wikipedia.org/wiki/Gnaro) for "nest".
- **Nice logo, did you design it yourself?** Kind of, I asked Midjourney to come up with some designs.

## TODO

- Reorder btree.h/c, comments in all H
- Name and order parameters in functions
- Ensure logging and comments are consistent, exhaustive and useful
