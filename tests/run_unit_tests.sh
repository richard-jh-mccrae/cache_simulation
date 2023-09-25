#!/bin/bash

TOP_LVL="$HOME/projects/cache_simulator"

if [ -e "cache_tests" ]; then
    rm cache_tests
fi
gcc -o cache_test test.c $TOP_LVL/cache_sim.c /usr/local/src/unity.c -I$TOP_LVL
./cache_test
