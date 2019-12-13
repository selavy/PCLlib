#!/bin/bash

BIN=./build/
OUTPUT=leak_check.output

valgrind ${BIN}/tests/unittest > /dev/null 2> $OUTPUT
if [ $? -ne 0 ]
then
    echo "Failed to run unit tests!" >&2;
    exit 1
fi

# TODO: check return code
grep --quiet "no leaks are possible" $OUTPUT
if [ $? -eq 0 ]; then
    echo "Passed.";
    rm $OUTPUT;
else
    echo "Failed. Inspect $OUTPUT" >&2;
    exit 1;
fi
