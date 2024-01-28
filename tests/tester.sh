#!/bin/bash

# Colors
green='\033[0;32m'
red='\033[0;31m'
reset='\033[0m'
blue='\033[34m'
orange='\033[38;5;214m'
yellow='\033[33m'

# Default values
verbose=false
cost=false
info=false
test_parent_dir="$(pwd)"
total_cost=0
num_failed_tests=0
num_total_tests=0
failed_test_names=()

compiler_path="../my_compiler/compiler"

log() {
    if [ "$verbose" = true ]; then
        echo -e "$1"
    fi
}

info() {
    if [ "$info" = true ]; then
        echo -e "$1"
    fi
}

create_directory() {
    if [ ! -d $1 ]; then
        info "CREATING DIRECTORY: $1"
        mkdir $1
    fi  
}

# Function to execute first command and check its execution
compile_file() {
    log "Running Stage One: $1"

    $1

    # Check the exit status of the command
    if [ $? -eq 0 ]; then
        info "ONE:.....${green}[PASSED]${reset}"
    else
        info "ONE:.....${red}[FAILED]${reset}"
        ((num_failed_tests++))
        failed_test_names+=("$2")
        # exit 1
    fi
}

compile_error_file() {
    log "Running Stage One: $1"

    compile_command="$1 2>&1"

    output="$($1 2>&1)"

  # Check the exit status of the command
    if [ $? -ne 0 ]; then        
        info "ONE:.....${green}[PASSED]${reset}"
    else
        info "ONE:.....${red}[FAILED]${reset}"
        ((num_failed_tests++))
        failed_test_names+=("$2")
        # exit 1
    fi

    echo "$output" | grep -o "[0-9]*" > $3
    log "Captured error message:"
    log "$output"
}

# Function to execute second command with keyboard input from a file
run_virtual_machine() {
    log "Running Stage Two: $1"

    # Execute the second command with input from a file
    test_dir="$(dirname $2)"
    test_input="$test_dir/input/$3.txt"

    # Check if input file exists for this test case
    if [ -f "$test_input" ]; then
        # Capture the output of the second command
        output=$($1 < $test_input)
    else
        # Execute the second command without input file
        output=$($1)
    fi

    if [ $? -eq 0 ]; then
        info "TWO:.....${green}[PASSED]${reset}"
    else
        info "TWO:.....${red}[FAILED]${reset}"
        ((num_failed_tests++))
        failed_test_names+=("$3")
        # exit 1
    fi

    # Extract lines starting with "> " and write them into a separate file
    echo "$output" | grep -o "> [0-9]*" > $4

    # Extract and accumulate cost
    test_cost=$(echo "$output" | grep -oP '\033\[31m([0-9]| )+' | perl -pe 's/\033\[31m|\033\[34m//g; s/\s//g;')
    total_cost=$((total_cost + test_cost))

    # Display the captured output
    log "Captured output for $3:"
    log "$output"

    if [ "$cost" = true ]; then
        echo -e "[$3] cost: ${orange}$test_cost ${reset}"
    fi
}

compare_output() {
    if cmp -s "$1" "$2"; then
        info "THREE:.....${green}[PASSED]${reset}"
    else
        info "THREE:.....${red}[FAILED]${reset}"
        ((num_failed_tests++))
        failed_test_names+=("$(basename "$1" .out)")
        # exit 1
    fi
}

test_error() {
    test_name=$1
    test_file=$2
    compiler_output_file=$3
    test_dir=$4
    error_output_file="$test_dir/output/$test_name.err"
    error_ref_file="$test_dir/ref/$test_name.err"
    compile_error_file "$compiler_path $test_file $compiler_output_file" "$test_name" "$error_output_file"
    compare_output "$error_output_file" "$error_ref_file"
}

_test() {
    # Test Stage One
    test_name=$1
    test_file=$2
    compiler_output_file=$3
    test_dir=$4
    compile_file "$compiler_path $test_file $compiler_output_file" "$test_name"

    # Test Stage Two
    vm_output_file="$test_output_dir/$test_name.out"
    run_virtual_machine "../virtual_machine/maszyna-wirtualna $compiler_output_file" "$test_file" "$test_name" "$vm_output_file"
    
    ref_file="$test_dir/ref/$test_name.out"

    # Test Stage Three
    if [ -f "$vm_output_file" ] && [ -f "$ref_file" ]; then
        compare_output "$vm_output_file" "$ref_file"
    fi
}

while getopts ":vci" opt; do
    case ${opt} in
        v )
            verbose=true
            ;;
        c )
            cost=true
            ;;
        i )
            info=true
            ;;
        \? )
            echo "Usage: $0 [-v] [-c] (optional -v for verbose output, -c to display costs, -i to get information about each failed and passed test)"
            exit 1
            ;;
        * )
            echo "Unknown option: -$OPTARG"
            echo "Usage: $0 [-v] [-c] (optional -v for verbose output, -c to display costs, -i to get information about each failed and passed test)"
            exit 1
            ;;
    esac
done

# Shift the positional parameters to skip processed options
shift $((OPTIND - 1))

# Loop through directories within the parent directory
for test_dir in "$test_parent_dir"/*/; do
    test_dir=$(realpath "$test_dir")
    
    # Skip the loop if the test_dir is the current directory
    test_output_dir="$test_dir/output"

    create_directory "$test_output_dir"

    info "RUNNING TESTS IN DIRECTORY: $test_dir"
    info

    # Loop through test case files in the specified directory
    for test_file in "$test_dir"/*.imp; do
        if [ -f "$test_file" ]; then
            ((num_total_tests++))
            test_name=$(basename "$test_file" .imp)
            compiler_output_file="$test_output_dir/$test_name.mr"

            info "TEST: [$test_name]"

            if [[ "$test_file" == *"error"* ]]; then
                test_error "$test_name" "$test_file" "$compiler_output_file" "$test_dir"
            else
                _test "$test_name" "$test_file" "$compiler_output_file" "$test_dir"
            fi

            info
        fi
    done
done

if [ "$cost" = true ]; then
    echo -e "Total cost: ${orange}$total_cost${reset}"
fi

num_passed_tests=$((num_total_tests - num_failed_tests))
if [ "$num_passed_tests" -eq "$num_total_tests" ]; then
    echo -e "${green}Tests passed: $num_passed_tests/$num_total_tests"
    echo -e "All tests passed${reset}"
    exit 0
else
    echo -e "${red}Tests passed: $num_passed_tests/$num_total_tests"
    echo -e "Failed tests names: ${failed_test_names[@]}${reset}"
    exit 1
fi
