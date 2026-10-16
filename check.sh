#!/usr/bin/env bash
# 给 git bisect run 用：0 = 正常，1 = 有 bug，125 = 无法测试
g++ -std=c++17 -fsanitize=address -g -o /tmp/bisect_test tests/test_frame.cpp codec.cpp >/dev/null 2>&1 || exit 125
/tmp/bisect_test >/dev/null 2>&1
exit $?
