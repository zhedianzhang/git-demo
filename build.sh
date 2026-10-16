#!/usr/bin/env bash
set -eu
mkdir -p build
g++ -std=c++17 -Wall -Wextra -fsanitize=address -g -o build/test_frame tests/test_frame.cpp codec.cpp
./build/test_frame
