#!/bin/bash

# Path to the hw5 directory
HW5_DIR="./hw5.out"
TESTS_DIR="hw5/tests"

# Loop through all test input files
for test_in in $TESTS_DIR/*.in; do
    # Extract test name (e.g., test2 from test2.in)
    test_name=$(basename $test_in .in)
    
    # Output files
    test_out="$TESTS_DIR/$test_name.out"
    temp_ll="$TESTS_DIR/$test_name.ll"
    temp_res="$TESTS_DIR/$test_name.res"

    echo "Running $test_name..."
    
    # Run the test
    $HW5_DIR < $test_in >& $temp_ll
    lli $temp_ll > $temp_res
    
    # Compare results
    if diff $temp_res $test_out > /dev/null; then
        echo "$test_name: PASSED"
    else
        echo "$test_name: FAILED"
        echo "Differences:"
        diff $temp_res $test_out
        # Exit the script on the first failure
        exit 1
    fi



    echo ""
done
