# P4-lang

[![Tests](https://github.com/blitzher/P4-lang/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/blitzher/P4-lang/actions/workflows/c-cpp.yml)


This is the repository for the language, Epicr, developed
by a group of students at AAU during the 4th semester.

## Language
The language is intended for food bloggers, people who cook and
anyone interested in writing and sharing recipies in an exchangeable
and nice looking format.

Additionally, it provides

- Static analysis of ingredient amounts and units
- Referencing other recipies as ingredients
- Generating ingredients lists for recipies
- HTML templates for output
- Translating between imperial and metric units

## Examples

Examples can be found in the examples directory.

## Compiling

Simply clone the repository, and compile with Clang++ and Make, either by
typing `make bin/main` to just compile, `make run` to compile and run on 
an example recipe, namely [Carbonara](examples/Carbonara.rcp).

## Tests

Testing is done with a developed unit testing library, located in `src/testing`.
To run the tests, type `make test`.
The tests are set up with Continuous Integration with Github, and are run on
Windows and Ubuntu on every push.



