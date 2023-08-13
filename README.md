# <img src="./docs/gnaro.png" style="width:40px;padding-right:10px;margin-bottom:-8px;"> gnaro
A Database similar to SQLite written in C for science.

## Usage

`gnaro` is currently empty. It can be run to display a simple welcome message (with the optional `-v` for verbose output):

```bash
$ sudo ./bin/gnaro [-v]

22:08:41 INFO  ./src/gnaro.c:57: Welcome to Gnaro!
22:08:55 INFO  ./src/gnaro.c:61: freeing resources...
22:08:55 INFO  ./src/gnaro.c:66: so long and thanks for all the wasps
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
```

Furthermore, the project includes a [Visual Studio Code](https://code.visualstudio.com/) configuration in `.vscode/` that can be used to run the built-in debugger.

## Structure

The project is structured as follows:

```txt
├── .devcontainer       configuration for GitHub Codespaces
├── .github             configuration GitHub Actions and other GitHub features
├── .vscode             configuration for Visual Studio Code
├── bin                 the executable (created by make)
├── build               intermediate build files e.g. *.o (created by make)
├── docs                documentation
├── include             header files
├── lib                 third-party libraries
├── scripts             scripts for setup and other tasks
├── src                 C source files
│   ├── gnaro.c         (main) Entry point for the CLI
│   └── *.c
├── tests               contains tests
├── .clang-format       configuration for the formatter
├── .clang-tidy         configuration for the linter
├── .gitignore
├── LICENSE
├── Makefile
└── README.md
```

## Testing and documentation

At the moment, the project does not contain any automated tests or tools to document the code.
In the future, suitable tools for automated testing and documentation might be added.

## Limitations

1.

## Improvements

- CMake and Conan are industry standards, so they should be used eventually instead of Make and the current build system. Unfortunately, CMake and Conan also add a lot of complexity which is not needed at this time.

## Credits

Some of the resources that have been used to develop `gnaro` are:

-

## FAQ

- **Why C?** I haven't written much C since college and nostalgia got me.
- **What does "gnaro" mean?** It's [Venetian](https://vec.wikipedia.org/wiki/Gnaro) (my native language) for "nest".
- **Nice logo, did you design it yourself?** Kind of, I asked Midjourney to come up with some designs.
