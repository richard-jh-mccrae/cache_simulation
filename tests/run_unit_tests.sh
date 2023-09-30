#!/bin/bash

TOP_LVL="$HOME/projects/cache_simulator"

if [ -e "unit_tests" ]; then
    rm unit_tests
fi
gcc -o unit_tests unit_test.c $TOP_LVL/cache_sim.c /usr/local/src/unity.c -I$TOP_LVL
./unit_tests
