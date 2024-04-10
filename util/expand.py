#!/usr/local/bin/python

from pathlib import Path
import argparse
import os
import re
import sys

library_directories = list(
    map(Path, os.environ.get("CPLUS_INCLUDE_PATH").split(":")))
if library_directories is None:
    library_directories = []

# library_pattern = re.compile(r"\s*(#include\s*[\"<](.*)[\">])\s*")
library_pattern = re.compile(r"\s*(#include\s*[\"](.*)[\"])\s*")
std_library_set = set([
    "bits/stdc++.h",
    "intrin.h",
    "cassert",
    "cctype",
    "cerrno",
    "cfloat",
    "ciso646",
    "climits",
    "clocale",
    "cmath",
    "csetjmp",
    "csignal",
    "cstdarg",
    "cstddef",
    "cstdio",
    "cstdlib",
    "cstring",
    "ctime",
    "cwchar",
    "cwctype",
    "ccomplex",
    "cfenv",
    "cinttypes",
    "cstdalign",
    "cstdbool",
    "cstdint",
    "ctgmath",
    "cuchar",
    "algorithm",
    "bitset",
    "complex",
    "deque",
    "exception",
    "fstream",
    "functional",
    "iomanip",
    "ios",
    "iosfwd",
    "iostream",
    "istream",
    "iterator",
    "limits",
    "list",
    "locale",
    "map",
    "memory",
    "new",
    "numeric",
    "ostream",
    "queue",
    "set",
    "sstream",
    "stack",
    "stdexcept",
    "streambuf",
    "string",
    "typeinfo",
    "utility",
    "valarray",
    "vector",
    "array",
    "atomic",
    "chrono",
    "codecvt",
    "condition_variable",
    "forward_list",
    "future",
    "initializer_list",
    "mutex",
    "random",
    "ratio",
    "regex",
    "scoped_allocator",
    "system_error",
    "thread",
    "tuple",
    "typeindex",
    "type_traits",
    "unordered_map",
    "unordered_set",
    "shared_mutex",
    "any",
    "charconv",
    "execution",
    "filesystem",
    "optional",
    "memory_resource",
    "string_view",
    "variant",
    "barrier",
    "bit",
    "compare",
    "concepts",
    "coroutine",
    "latch",
    "numbers",
    "ranges",
    "span",
    "stop_token",
    "semaphore",
    "source_location",
    "syncstream",
    "version"
])
used_library_set: set[Path] = set()
current_path_stack: list[Path] = [Path("./")]


def is_expanded_line(line: str) -> bool:
    return library_pattern.match(line) is not None


def parse_expanded_line(line: str) -> str:
    assert is_expanded_line(line)
    match: re.Match = library_pattern.match(line)
    filename = match.group(2)
    if filename in std_library_set:
        return line
    path = get_library_path(filename)
    # ここ消したらatcoder/*もexpandされる
    if path.parent.name == "atcoder":
        return line
    if path in used_library_set:
        return ""
    used_library_set.add(path)
    current_path_stack.append(path.parent)
    source = parse_file(path)
    current_path_stack.pop()
    return source


def get_library_path(filename: str):
    global library_directories
    for library_directory in (library_directories + [current_path_stack[-1]]):
        path = Path(library_directory / filename)
        if path.exists():
            return path.resolve()

    print(f"{filename} is not found", file=sys.stderr)
    exit()


def get_file(path: Path) -> str:
    with open(path) as f:
        source: str = f.read()
    return source


def parse_file(path: Path) -> str:
    lines: list[str] = get_file(path).splitlines()
    return parse_lines(lines)


def parse_lines(lines: list[str]) -> str:
    source: list[str] = []
    for line in lines:
        if is_expanded_line(line):
            source.append(parse_expanded_line(line))
        else:
            source.append(line)
    return "\n".join(source)


def get_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("filename", nargs="?")
    parser.add_argument("-I", "--include", dest="include_paths", nargs="*")
    return parser.parse_args()


def main():
    global library_directories
    args = get_args()
    if args.include_paths:
        library_directories += args.include_paths
    if args.filename:
        source = parse_file(args.filename)
    else:
        source = parse_lines(list(map(str.rstrip, sys.stdin.readlines())))
    print(source)


if __name__ == "__main__":
    main()
