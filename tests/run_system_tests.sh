#!/bin/bash

TOP_LVL="$HOME/projects/cache_simulator"

if [ -e "system_test" ]; then
    rm system_test
fi
gcc -o system_test $TOP_LVL/cache_sim.c

echo "Using mem_trace1.txt"
echo " "

echo "--- Direct Mapped ---"

echo "128B, DM, UC"
./system_test 128 dm uc
echo "Expected 5 accesses, 0 hits"
echo "----"
echo ""
echo ""

echo "4096B, DM, UC"
./system_test 4096 dm uc
echo "Expected 5 accesses, 1 hit"
echo "----"
echo ""
echo ""

echo "128B, DM, SC"
./system_test 128 dm uc
echo "Expect 5 accesses, 0 hits"
echo "----"
echo ""
echo ""

echo "4096B, DM, SC"
./system_test 4096 dm uc
echo "Expect 5 accesses, 1 hit"
echo "----"

echo "--- Fully Associative ---"

echo "128B, FA, UC"
./system_test 128 fa uc
echo "Expected 5 accesses, 1 hit"
echo "----"
echo ""
echo ""

echo "4096B, fa, UC"
./system_test 4096 fa uc
echo "Expected 5 accesses, 1 hit"
echo "----"
echo ""
echo ""

echo "128B, FA, SC"
./system_test 128 fa uc
echo "Expected 5 accesses, 1 hit"
echo "----"
echo ""
echo ""

echo "4096B, FA, SC"
./system_test 4096 fa uc
echo "Expected 5 accesses, 1 hit"
echo "----"
