#!/bin/bash

BIN=./build/
OUTPUT=leak_check.output
failures=0

ninja -C ${BIN} || exit 1;

#
# C++ Tests
#
echo "Running C++ tests..."
valgrind ${BIN}/tests/unittest > /dev/null 2> $OUTPUT
if [ $? -ne 0 ]
then
    echo "Failed to run unit tests. Does the executable exist?" >&2;
    exit 1
fi

grep --quiet "no leaks are possible" $OUTPUT
if [ $? -ne 0 ];
then
    echo "Failed. Inspect $OUTPUT" >&2;
    failures=failures + 1;
else
    echo "Passed."
    rm $OUTPUT
fi;

#
# C90 Tests
#
C90_TESTS=(test-pclvector-c90)
for exe in ${C90_TESTS[@]};
do
    echo "Running ${exe}..."
    output=leak_check.${exe}.output
    valgrind ${BIN}/tests/${exe} > /dev/null 2> $output
    if [ $? -ne 0 ]
    then
        echo "Failed to run test. Does the executable exist?" >&2;
        exit 1
    fi
    grep --quiet "no leaks are possible" $output
    if [ $? -eq 0 ]
    then
        echo "Passed.";
        rm $output
    else
        echo "Failed. Inspect $output" >&2;
        failures=failures + 1;
    fi;
done;

if [ $failures -ne 0 ]
then
    echo "Had ${failures} failures. :-(";
    exit 1
else
    exit 0
fi
