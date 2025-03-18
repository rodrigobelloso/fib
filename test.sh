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
  local test_name="$description (n=$number)"

  echo -n "Testing $test_name: "
  
  output=$(./fib $number)
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
run_test 0 "Fibonacci Number 0: 0" "Fibonacci 0"
((total_tests++))
[ $? -eq 0 ] && ((passed_tests++))

run_test 1 "Fibonacci Number 1: 1" "Fibonacci 1"
((total_tests++))
[ $? -eq 0 ] && ((passed_tests++))

run_test 10 "Fibonacci Number 10: 55" "Fibonacci 10"
((total_tests++))
[ $? -eq 0 ] && ((passed_tests++))

run_test 20 "Fibonacci Number 20: 6765" "Fibonacci 20"
((total_tests++))
[ $? -eq 0 ] && ((passed_tests++))

echo -e "\n=== Large number test ==="
run_test 100 "Fibonacci Number 100: 354224848179261915075" "Fibonacci 100"
((total_tests++))
[ $? -eq 0 ] && ((passed_tests++))

echo -e "\n=== Error handling tests ==="
echo -n "Testing without arguments: "
./fib >/dev/null 2>&1
if [ $? -ne 0 ]; then
  echo -e "${GREEN}SUCCESS: Program correctly detected the error${NC}"
  ((passed_tests++))
else
  echo -e "${RED}FAILED: Program should have failed without arguments${NC}"
  failed_tests+=("No arguments - Did not fail as expected")
fi
((total_tests++))

echo -n "Testing with non-numeric argument: "
./fib abc >/dev/null 2>&1
if [ $? -ne 0 ]; then
  echo -e "${GREEN}SUCCESS: Program correctly detected the error${NC}"
  ((passed_tests++))
else
  echo -e "${RED}FAILED: Program should have failed with non-numeric input${NC}"
  failed_tests+=("Non-numeric argument - Did not fail as expected")
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
    echo -e "  ${RED}$(($i+1)). ${failed_tests[$i]}${NC}"
  done
  echo
  echo -e "${RED}SOME TESTS FAILED (${#failed_tests[@]} of $total_tests)${NC}"
  exit 1
else
  echo -e "${GREEN}ALL TESTS PASSED SUCCESSFULLY${NC}"
  exit 0
fi
