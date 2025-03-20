#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

failed_tests=()

run_test() {
  local number=$1
  local expected_result=$2
  local description=$3
  local options=${4:-""}
  local test_name="$description (n=$number${options:+ }${options})"

  echo -n "Testing $test_name: "

  local args=()
  
  if [ -n "$options" ]; then
    read -ra opt_array <<< "$options"
    for opt in "${opt_array[@]}"; do
      args+=("$opt")
    done
  fi
  
  args+=("$number")
  
  output=$(./fib "${args[@]}")
  exit_code=$?
  
  if [ $exit_code -ne 0 ]; then
    echo -e "${RED}ERROR: Program terminated with exit code $exit_code${NC}"
    failed_tests+=("$test_name - Exit code $exit_code")
    return 1
  fi
  
  if echo "$output" | grep -q "$expected_result"; then
    echo -e "${GREEN}SUCCESS${NC}"
    return 0
  else
    echo -e "${RED}FAILED${NC}"
    echo "  Expected: $expected_result"
    echo "  Obtained: $output"
    failed_tests+=("$test_name - Incorrect value")
    return 1
  fi
}

total_tests=0
passed_tests=0

echo "=== Tests with known Fibonacci values ==="
if run_test 0 "Fibonacci Number 0: 0" "Fibonacci 0"; then
  ((passed_tests++))
fi
((total_tests++))

if run_test 1 "Fibonacci Number 1: 1" "Fibonacci 1"; then
  ((passed_tests++))
fi
((total_tests++))

if run_test 10 "Fibonacci Number 10: 55" "Fibonacci 10"; then
  ((passed_tests++))
fi
((total_tests++))

if run_test 20 "Fibonacci Number 20: 6765" "Fibonacci 20"; then
  ((passed_tests++))
fi
((total_tests++))

echo -e "\n=== Large number test ==="
if run_test 100 "Fibonacci Number 100: 354224848179261915075" "Fibonacci 100"; then
  ((passed_tests++))
fi
((total_tests++))

echo -e "\n=== Flag tests ==="

echo -n "Testing with -t flag: "
output=$(./fib -t 30)
exit_code=$?

if [ $exit_code -ne 0 ]; then
  echo -e "${RED}ERROR: Program terminated with exit code $exit_code${NC}"
  failed_tests+=("Time flag test - Exit code $exit_code")
else
  if echo "$output" | grep -q "Calculation Time: "; then
    echo -e "${GREEN}SUCCESS: Time output detected${NC}"
    ((passed_tests++))
  else
    echo -e "${RED}FAILED: No time output detected${NC}"
    failed_tests+=("Time flag test - No time output")
  fi
fi
((total_tests++))

echo -n "Testing with --time flag: "
output=$(./fib --time 30)
exit_code=$?

if [ $exit_code -ne 0 ]; then
  echo -e "${RED}ERROR: Program terminated with exit code $exit_code${NC}"
  failed_tests+=("Long time flag test - Exit code $exit_code")
else
  if echo "$output" | grep -q "Calculation Time: "; then
    echo -e "${GREEN}SUCCESS: Time output detected${NC}"
    ((passed_tests++))
  else
    echo -e "${RED}FAILED: No time output detected${NC}"
    failed_tests+=("Long time flag test - No time output")
  fi
fi
((total_tests++))

echo -n "Testing with -r flag: "
output=$(./fib -r 20)
expected="6765"
exit_code=$?

if [ $exit_code -ne 0 ]; then
  echo -e "${RED}ERROR: Program terminated with exit code $exit_code${NC}"
  failed_tests+=("Raw flag test - Exit code $exit_code")
else
  output=$(echo "$output" | tr -d '[:space:]')
  if [ "$output" == "$expected" ]; then
    echo -e "${GREEN}SUCCESS: Raw output without prefix${NC}"
    ((passed_tests++))
  else
    echo -e "${RED}FAILED: Output contains prefix or incorrect value${NC}"
    echo "  Expected: $expected"
    echo "  Obtained: $output"
    failed_tests+=("Raw flag test - Incorrect format")
  fi
fi
((total_tests++))

echo -n "Testing with --raw flag: "
output=$(./fib --raw 20)
expected="6765"
exit_code=$?

if [ $exit_code -ne 0 ]; then
  echo -e "${RED}ERROR: Program terminated with exit code $exit_code${NC}"
  failed_tests+=("Long raw flag test - Exit code $exit_code")
else
  output=$(echo "$output" | tr -d '[:space:]')
  if [ "$output" == "$expected" ]; then
    echo -e "${GREEN}SUCCESS: Raw output without prefix${NC}"
    ((passed_tests++))
  else
    echo -e "${RED}FAILED: Output contains prefix or incorrect value${NC}"
    echo "  Expected: $expected"
    echo "  Obtained: $output"
    failed_tests+=("Long raw flag test - Incorrect format")
  fi
fi
((total_tests++))

echo -n "Testing with -o flag: "
temp_output_file=$(mktemp)
./fib -o "$temp_output_file" 20
exit_code=$?

if [ $exit_code -ne 0 ]; then
  echo -e "${RED}ERROR: Program terminated with exit code $exit_code${NC}"
  failed_tests+=("Output file test - Exit code $exit_code")
else
  if [ -f "$temp_output_file" ]; then
    file_content=$(cat "$temp_output_file")
    if echo "$file_content" | grep -q "Fibonacci Number 20: 6765"; then
      echo -e "${GREEN}SUCCESS: File output correct${NC}"
      ((passed_tests++))
    else
      echo -e "${RED}FAILED: File content incorrect${NC}"
      echo "  Expected to contain: Fibonacci Number 20: 6765"
      echo "  File content: $file_content"
      failed_tests+=("Output file test - Incorrect content")
    fi
    rm -f "$temp_output_file"
  else
    echo -e "${RED}FAILED: Output file not created${NC}"
    failed_tests+=("Output file test - File not created")
  fi
fi
((total_tests++))

echo -n "Testing with --output flag: "
temp_output_file=$(mktemp)
./fib --output "$temp_output_file" 20
exit_code=$?

if [ $exit_code -ne 0 ]; then
  echo -e "${RED}ERROR: Program terminated with exit code $exit_code${NC}"
  failed_tests+=("Long output file test - Exit code $exit_code")
else
  if [ -f "$temp_output_file" ]; then
    file_content=$(cat "$temp_output_file")
    if echo "$file_content" | grep -q "Fibonacci Number 20: 6765"; then
      echo -e "${GREEN}SUCCESS: File output correct${NC}"
      ((passed_tests++))
    else
      echo -e "${RED}FAILED: File content incorrect${NC}"
      echo "  Expected to contain: Fibonacci Number 20: 6765"
      echo "  File content: $file_content"
      failed_tests+=("Long output file test - Incorrect content")
    fi
    rm -f "$temp_output_file"
  else
    echo -e "${RED}FAILED: Output file not created${NC}"
    failed_tests+=("Long output file test - File not created")
  fi
fi
((total_tests++))

echo -n "Testing combining flags (-t -r): "
output=$(./fib -t -r 20)
exit_code=$?

if [ $exit_code -ne 0 ]; then
  echo -e "${RED}ERROR: Program terminated with exit code $exit_code${NC}"
  failed_tests+=("Combined flags test - Exit code $exit_code")
else
  if echo "$output" | grep -q "^6765$" && echo "$output" | grep -q "Calculation Time:"; then
    echo -e "${GREEN}SUCCESS: Combined flags working correctly${NC}"
    ((passed_tests++))
  else
    echo -e "${RED}FAILED: Combined flags not working correctly${NC}"
    echo "  Expected format: raw output (6765) and calculation time"
    echo "  Obtained: $output"
    failed_tests+=("Combined flags test - Incorrect format")
  fi
fi
((total_tests++))

echo -n "Testing with flag at the end: "
output=$(./fib 25 -t)
exit_code=$?

if [ $exit_code -ne 0 ]; then
  echo -e "${RED}ERROR: Program terminated with exit code $exit_code${NC}"
  failed_tests+=("Time flag at end - Exit code $exit_code")
else
  if echo "$output" | grep -q "Calculation Time: "; then
    echo -e "${GREEN}SUCCESS: Time output detected${NC}"
    ((passed_tests++))
  else
    echo -e "${RED}FAILED: No time output detected${NC}"
    failed_tests+=("Time flag at end - No time output")
  fi
fi
((total_tests++))

echo -n "Testing all flags together: "
temp_output_file=$(mktemp)
./fib -t -r -o "$temp_output_file" 15
exit_code=$?

if [ $exit_code -ne 0 ]; then
  echo -e "${RED}ERROR: Program terminated with exit code $exit_code${NC}"
  failed_tests+=("All flags test - Exit code $exit_code")
else
  if [ -f "$temp_output_file" ]; then
    file_content=$(cat "$temp_output_file")
    if grep -q "^610$" <<< "$(echo "$file_content" | head -n1)" && grep -q "Calculation Time:" "$temp_output_file"; then
      echo -e "${GREEN}SUCCESS: All flags working correctly${NC}"
      ((passed_tests++))
    else
      echo -e "${RED}FAILED: All flags not working correctly${NC}"
      echo "  Expected: raw output (610) and calculation time"
      echo "  File content: $file_content"
      failed_tests+=("All flags test - Incorrect format")
    fi
    rm -f "$temp_output_file"
  else
    echo -e "${RED}FAILED: Output file not created${NC}"
    failed_tests+=("All flags test - File not created")
  fi
fi
((total_tests++))

echo -e "\n=== Error handling tests ==="
echo -n "Testing without arguments: "
if ! ./fib >/dev/null 2>&1; then
  echo -e "${GREEN}SUCCESS: Program correctly detected the error${NC}"
  ((passed_tests++))
else
  echo -e "${RED}FAILED: Program should have failed without arguments${NC}"
  failed_tests+=("No arguments - Did not fail as expected")
fi
((total_tests++))

echo -n "Testing with non-numeric argument: "
if ! ./fib abc >/dev/null 2>&1; then
  echo -e "${GREEN}SUCCESS: Program correctly detected the error${NC}"
  ((passed_tests++))
else
  echo -e "${RED}FAILED: Program should have failed with non-numeric input${NC}"
  failed_tests+=("Non-numeric argument - Did not fail as expected")
fi
((total_tests++))

echo -n "Testing with invalid flag usage: "
if ! ./fib -t -x 10 >/dev/null 2>&1; then
  echo -e "${GREEN}SUCCESS: Program correctly detected the error${NC}"
  ((passed_tests++))
else
  echo -e "${RED}FAILED: Program should have failed with invalid flag usage${NC}"
  failed_tests+=("Invalid flag usage - Did not fail as expected")
fi
((total_tests++))

echo -n "Testing missing argument for -o flag: "
if ! ./fib -o >/dev/null 2>&1; then
  echo -e "${GREEN}SUCCESS: Program correctly detected the error${NC}"
  ((passed_tests++))
else
  echo -e "${RED}FAILED: Program should have failed with missing -o argument${NC}"
  failed_tests+=("Missing -o argument - Did not fail as expected")
fi
((total_tests++))

echo -e "\n=== Performance test ==="
echo "Calculating Fibonacci(1000)..."
time ./fib 1000 >/dev/null
perf_result=$?
((total_tests++))
if [ $perf_result -eq 0 ]; then
  ((passed_tests++))
else
  failed_tests+=("Performance test - Failed with code $perf_result")
fi

echo -e "\n=== Test Summary ==="
echo -e "Total tests: $total_tests"
echo -e "Successful tests: $passed_tests"

if [ ${#failed_tests[@]} -gt 0 ]; then
  echo -e "\n${YELLOW}Failed tests (${#failed_tests[@]}):${NC}"
  for ((i=0; i<${#failed_tests[@]}; i++)); do
    echo -e "  ${RED}$((i+1)). ${failed_tests[$i]}${NC}"
  done
  echo
  echo -e "${RED}SOME TESTS FAILED (${#failed_tests[@]} of $total_tests)${NC}"
  exit 1
else
  echo -e "${GREEN}ALL TESTS PASSED SUCCESSFULLY${NC}"
  exit 0
fi
