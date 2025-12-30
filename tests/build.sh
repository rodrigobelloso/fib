#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

failed_tests=()

cd ..

# Check for required dependencies
echo -e "${YELLOW}=== Checking Testing Dependencies ===${NC}"

# Check if the fib executable exists
if [ ! -f "./fib" ]; then
  echo -e "${RED}✗ fib executable not found${NC}"
  echo -e "\n${RED}ERROR: The 'fib' executable is missing${NC}"
  echo -e "Please build the executable before running tests."
  echo ""
  exit 1
else
  echo -e "${GREEN}✓ fib executable found${NC}"
fi

# Check if the executable is actually executable
if [ ! -x "./fib" ]; then
  echo -e "${RED}✗ fib is not executable${NC}"
  echo -e "\n${RED}ERROR: The 'fib' file exists but is not executable${NC}"
  echo -e "Please set executable permissions on the file."
  echo ""
  exit 1
else
  echo -e "${GREEN}✓ fib is executable${NC}"
fi

echo ""

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
if run_test 0 "Fibonacci Number 0 (decimal): 0" "Fibonacci 0"; then
  ((passed_tests++))
fi
((total_tests++))

if run_test 1 "Fibonacci Number 1 (decimal): 1" "Fibonacci 1"; then
  ((passed_tests++))
fi
((total_tests++))

if run_test 10 "Fibonacci Number 10 (decimal): 55" "Fibonacci 10"; then
  ((passed_tests++))
fi
((total_tests++))

if run_test 20 "Fibonacci Number 20 (decimal): 6765" "Fibonacci 20"; then
  ((passed_tests++))
fi
((total_tests++))

echo -e "\n=== Large number test ==="
if run_test 100 "Fibonacci Number 100 (decimal): 354224848179261915075" "Fibonacci 100"; then
  ((passed_tests++))
fi
((total_tests++))

echo -e "\n=== Algorithm tests ==="
if run_test 20 "Fibonacci Number 20 (decimal): 6765" "Fibonacci iterative" "-a iter"; then
  ((passed_tests++))
fi
((total_tests++))

if run_test 20 "Fibonacci Number 20 (decimal): 6765" "Fibonacci recursive" "-a recur"; then
  ((passed_tests++))
fi
((total_tests++))

if run_test 20 "Fibonacci Number 20 (decimal): 6765" "Fibonacci matrix" "-a matrix"; then
  ((passed_tests++))
fi
((total_tests++))

echo -e "\n=== Long-form algorithm tests ==="
if run_test 20 "Fibonacci Number 20 (decimal): 6765" "Fibonacci iterative" "--algorithm iter"; then
  ((passed_tests++))
fi
((total_tests++))

if run_test 20 "Fibonacci Number 20 (decimal): 6765" "Fibonacci recursive" "--algorithm recur"; then
  ((passed_tests++))
fi
((total_tests++))

if run_test 20 "Fibonacci Number 20 (decimal): 6765" "Fibonacci matrix" "--algorithm matrix"; then
  ((passed_tests++))
fi
((total_tests++))

echo -e "\n=== Format tests ==="
if run_test 10 "Fibonacci Number 10 (hexadecimal): 0x37" "Fibonacci hex" "-f hex"; then
  ((passed_tests++))
fi
((total_tests++))

if run_test 10 "Fibonacci Number 10 (binary): 0b110111" "Fibonacci bin" "-f bin"; then
  ((passed_tests++))
fi
((total_tests++))

if run_test 10 "Fibonacci Number 10 (decimal): 55" "Fibonacci dec" "-f dec"; then
  ((passed_tests++))
fi
((total_tests++))

echo -e "\n=== Format with raw tests ==="
if run_test 10 "^55$" "Fibonacci raw dec" "-f dec -r"; then
  ((passed_tests++))
fi
((total_tests++))

if run_test 10 "^0x37$" "Fibonacci raw hex" "-f hex -r"; then
  ((passed_tests++))
fi
((total_tests++))

if run_test 10 "^0b110111$" "Fibonacci raw bin" "-f bin -r"; then
  ((passed_tests++))
fi
((total_tests++))

echo -e "\n=== Flag tests ==="

echo -n "Testing with -h flag: "
output=$(./fib -h)
exit_code=$?

if [ $exit_code -ne 0 ]; then
  echo -e "${RED}ERROR: Program terminated with exit code $exit_code${NC}"
  failed_tests+=("Help flag test - Exit code $exit_code")
else
  if echo "$output" | grep -q "Usage:" && echo "$output" | grep -q "Options:"; then
    echo -e "${GREEN}SUCCESS: Help message displayed${NC}"
    ((passed_tests++))
  else
    echo -e "${RED}FAILED: No help message detected${NC}"
    failed_tests+=("Help flag test - No help output")
  fi
fi
((total_tests++))

echo -n "Testing with --help flag: "
output=$(./fib --help)
exit_code=$?

if [ $exit_code -ne 0 ]; then
  echo -e "${RED}ERROR: Program terminated with exit code $exit_code${NC}"
  failed_tests+=("Long help flag test - Exit code $exit_code")
else
  if echo "$output" | grep -q "Usage:" && echo "$output" | grep -q "Options:"; then
    echo -e "${GREEN}SUCCESS: Help message displayed${NC}"
    ((passed_tests++))
  else
    echo -e "${RED}FAILED: No help message detected${NC}"
    failed_tests+=("Long help flag test - No help output")
  fi
fi
((total_tests++))

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

echo -n "Testing with -T flag (time only): "
output=$(./fib -T 100)
exit_code=$?

if [ $exit_code -ne 0 ]; then
  echo -e "${RED}ERROR: Program terminated with exit code $exit_code${NC}"
  failed_tests+=("Time only flag test - Exit code $exit_code")
else
  if echo "$output" | grep -q "Calculation Time: " && ! echo "$output" | grep -q "Fibonacci Number"; then
    echo -e "${GREEN}SUCCESS: Time only output detected${NC}"
    ((passed_tests++))
  else
    echo -e "${RED}FAILED: Expected only time output, no result${NC}"
    echo "  Output: $output"
    failed_tests+=("Time only flag test - Incorrect output")
  fi
fi
((total_tests++))

echo -n "Testing with --time-only flag: "
output=$(./fib --time-only 100)
exit_code=$?

if [ $exit_code -ne 0 ]; then
  echo -e "${RED}ERROR: Program terminated with exit code $exit_code${NC}"
  failed_tests+=("Long time only flag test - Exit code $exit_code")
else
  if echo "$output" | grep -q "Calculation Time: " && ! echo "$output" | grep -q "Fibonacci Number"; then
    echo -e "${GREEN}SUCCESS: Time only output detected${NC}"
    ((passed_tests++))
  else
    echo -e "${RED}FAILED: Expected only time output, no result${NC}"
    echo "  Output: $output"
    failed_tests+=("Long time only flag test - Incorrect output")
  fi
fi
((total_tests++))

echo -n "Testing -T with verbose flag: "
output=$(./fib -T -v 50 2>&1)
exit_code=$?

if [ $exit_code -ne 0 ]; then
  echo -e "${RED}ERROR: Program terminated with exit code $exit_code${NC}"
  failed_tests+=("Time only with verbose test - Exit code $exit_code")
else
  if echo "$output" | grep -q "Calculation Time: " && echo "$output" | grep -q "Initializing" && ! echo "$output" | grep -q "Fibonacci Number"; then
    echo -e "${GREEN}SUCCESS: Time only with verbose working correctly${NC}"
    ((passed_tests++))
  else
    echo -e "${RED}FAILED: Time only with verbose not working correctly${NC}"
    echo "  Expected: time output, verbose info, but no result"
    failed_tests+=("Time only with verbose test - Incorrect output")
  fi
fi
((total_tests++))

echo -n "Testing -T with different algorithms: "
output=$(./fib -T -a matrix 1000)
exit_code=$?

if [ $exit_code -ne 0 ]; then
  echo -e "${RED}ERROR: Program terminated with exit code $exit_code${NC}"
  failed_tests+=("Time only with algorithm test - Exit code $exit_code")
else
  if echo "$output" | grep -q "Calculation Time: " && ! echo "$output" | grep -q "Fibonacci Number"; then
    echo -e "${GREEN}SUCCESS: Time only with algorithm working correctly${NC}"
    ((passed_tests++))
  else
    echo -e "${RED}FAILED: Time only with algorithm not working correctly${NC}"
    failed_tests+=("Time only with algorithm test - Incorrect output")
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

echo -n "Testing with -v flag: "
output=$(./fib -v 10 2>&1)
exit_code=$?

if [ $exit_code -ne 0 ]; then
  echo -e "${RED}ERROR: Program terminated with exit code $exit_code${NC}"
  failed_tests+=("Verbose flag test - Exit code $exit_code")
else
  if echo "$output" | grep -q "Initializing" && echo "$output" | grep -q "Calculating"; then
    echo -e "${GREEN}SUCCESS: Verbose output detected${NC}"
    ((passed_tests++))
  else
    echo -e "${RED}FAILED: No verbose output detected${NC}"
    failed_tests+=("Verbose flag test - No verbose output")
  fi
fi
((total_tests++))

echo -n "Testing with --verbose flag: "
output=$(./fib --verbose 10 2>&1)
exit_code=$?

if [ $exit_code -ne 0 ]; then
  echo -e "${RED}ERROR: Program terminated with exit code $exit_code${NC}"
  failed_tests+=("Long verbose flag test - Exit code $exit_code")
else
  if echo "$output" | grep -q "Initializing" && echo "$output" | grep -q "Calculating"; then
    echo -e "${GREEN}SUCCESS: Verbose output detected${NC}"
    ((passed_tests++))
  else
    echo -e "${RED}FAILED: No verbose output detected${NC}"
    failed_tests+=("Long verbose flag test - No verbose output")
  fi
fi
((total_tests++))

echo -n "Testing with -o flag: "
temp_output_file="/tmp/fib_test_o_$$.txt"
./fib -o "$temp_output_file" 20
exit_code=$?

if [ $exit_code -ne 0 ]; then
  echo -e "${RED}ERROR: Program terminated with exit code $exit_code${NC}"
  failed_tests+=("Output file test - Exit code $exit_code")
else
  if [ -f "$temp_output_file" ]; then
    file_content=$(cat "$temp_output_file")
    if echo "$file_content" | grep -q "Fibonacci Number 20 (decimal): 6765"; then
      echo -e "${GREEN}SUCCESS: File output correct${NC}"
      ((passed_tests++))
    else
      echo -e "${RED}FAILED: File content incorrect${NC}"
      echo "  Expected to contain: Fibonacci Number 20 (decimal): 6765"
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
temp_output_file="/tmp/fib_test_output_$$.txt"
./fib --output "$temp_output_file" 20
exit_code=$?

if [ $exit_code -ne 0 ]; then
  echo -e "${RED}ERROR: Program terminated with exit code $exit_code${NC}"
  failed_tests+=("Long output file test - Exit code $exit_code")
else
  if [ -f "$temp_output_file" ]; then
    file_content=$(cat "$temp_output_file")
    if echo "$file_content" | grep -q "Fibonacci Number 20 (decimal): 6765"; then
      echo -e "${GREEN}SUCCESS: File output correct${NC}"
      ((passed_tests++))
    else
      echo -e "${RED}FAILED: File content incorrect${NC}"
      echo "  Expected to contain: Fibonacci Number 20 (decimal): 6765"
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

echo -e "\n=== Security validation tests ==="
echo -n "Testing path traversal prevention (../../../etc/passwd): "
output=$(./fib -o "../../../etc/passwd" 10 2>&1)
exit_code=$?

if [ $exit_code -ne 0 ]; then
  if echo "$output" | grep -q "Path traversal detected"; then
    echo -e "${GREEN}SUCCESS: Path traversal blocked${NC}"
    ((passed_tests++))
  else
    echo -e "${RED}FAILED: Wrong error message for path traversal${NC}"
    echo "  Expected error about path traversal"
    echo "  Got: $output"
    failed_tests+=("Path traversal test - Wrong error message")
  fi
else
  echo -e "${RED}FAILED: Path traversal not blocked${NC}"
  failed_tests+=("Path traversal test - Should have been blocked")
fi
((total_tests++))

echo -n "Testing system directory write prevention (/etc/test): "
output=$(./fib -o "/etc/test" 10 2>&1)
exit_code=$?

if [ $exit_code -ne 0 ]; then
  if echo "$output" | grep -q "Cannot write to system directory\|Output path must be in current directory"; then
    echo -e "${GREEN}SUCCESS: System directory write blocked${NC}"
    ((passed_tests++))
  else
    echo -e "${RED}FAILED: Wrong error message for system directory${NC}"
    echo "  Expected error about system directory or allowed paths"
    echo "  Got: $output"
    failed_tests+=("System directory test - Wrong error message")
  fi
else
  echo -e "${RED}FAILED: System directory write not blocked${NC}"
  failed_tests+=("System directory test - Should have been blocked")
fi
((total_tests++))

echo -n "Testing /tmp directory (should succeed): "
temp_test_file="/tmp/fib_test_$$.txt"
./fib -o "$temp_test_file" 10 2>&1
exit_code=$?

if [ $exit_code -eq 0 ]; then
  if [ -f "$temp_test_file" ]; then
    echo -e "${GREEN}SUCCESS: /tmp write allowed${NC}"
    ((passed_tests++))
    rm -f "$temp_test_file"
  else
    echo -e "${RED}FAILED: /tmp write should succeed but file not created${NC}"
    failed_tests+=("/tmp write test - File not created")
  fi
else
  echo -e "${RED}FAILED: /tmp write should be allowed${NC}"
  failed_tests+=("/tmp write test - Exit code $exit_code")
fi
((total_tests++))

echo -n "Testing current directory write (should succeed): "
temp_test_file="./fib_test_current_$$.txt"
./fib -o "$temp_test_file" 10 2>&1
exit_code=$?

if [ $exit_code -eq 0 ]; then
  if [ -f "$temp_test_file" ]; then
    echo -e "${GREEN}SUCCESS: Current directory write allowed${NC}"
    ((passed_tests++))
    rm -f "$temp_test_file"
  else
    echo -e "${RED}FAILED: Current directory write should succeed but file not created${NC}"
    failed_tests+=("Current directory write test - File not created")
  fi
else
  echo -e "${RED}FAILED: Current directory write should be allowed${NC}"
  failed_tests+=("Current directory write test - Exit code $exit_code")
fi
((total_tests++))

echo -n "Testing home directory write (should succeed): "
if [ -n "$HOME" ]; then
  temp_test_file="$HOME/fib_test_home_$$.txt"
  ./fib -o "$temp_test_file" 10 2>&1
  exit_code=$?

  if [ $exit_code -eq 0 ]; then
    if [ -f "$temp_test_file" ]; then
      echo -e "${GREEN}SUCCESS: Home directory write allowed${NC}"
      ((passed_tests++))
      rm -f "$temp_test_file"
    else
      echo -e "${RED}FAILED: Home directory write should succeed but file not created${NC}"
      failed_tests+=("Home directory write test - File not created")
    fi
  else
    echo -e "${RED}FAILED: Home directory write should be allowed${NC}"
    failed_tests+=("Home directory write test - Exit code $exit_code")
  fi
else
  echo -e "${YELLOW}SKIPPED: HOME environment variable not set${NC}"
fi
((total_tests++))

echo -n "Testing double slash prevention (path//file): "
output=$(./fib -o "test//file.txt" 10 2>&1)
exit_code=$?

if [ $exit_code -ne 0 ]; then
  if echo "$output" | grep -q "Path traversal detected"; then
    echo -e "${GREEN}SUCCESS: Double slash blocked${NC}"
    ((passed_tests++))
  else
    echo -e "${RED}FAILED: Wrong error message for double slash${NC}"
    echo "  Expected error about path traversal"
    echo "  Got: $output"
    failed_tests+=("Double slash test - Wrong error message")
  fi
else
  echo -e "${RED}FAILED: Double slash not blocked${NC}"
  failed_tests+=("Double slash test - Should have been blocked")
fi
((total_tests++))

echo -e "\n=== Combination tests ==="
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

echo -n "Testing combining flags (-t -v): "
output=$(./fib -t -v 10 2>&1)
exit_code=$?

if [ $exit_code -ne 0 ]; then
  echo -e "${RED}ERROR: Program terminated with exit code $exit_code${NC}"
  failed_tests+=("Combined verbose flags test - Exit code $exit_code")
else
  if echo "$output" | grep -q "Calculation Time:" && echo "$output" | grep -q "Initializing"; then
    echo -e "${GREEN}SUCCESS: Combined flags with verbose working correctly${NC}"
    ((passed_tests++))
  else
    echo -e "${RED}FAILED: Combined flags with verbose not working correctly${NC}"
    echo "  Expected: time output and verbose information"
    echo "  Obtained: $output"
    failed_tests+=("Combined verbose flags test - Incorrect output")
  fi
fi
((total_tests++))

echo -n "Testing algorithm with other flags (-a matrix -t): "
output=$(./fib -a matrix -t 15)
exit_code=$?

if [ $exit_code -ne 0 ]; then
  echo -e "${RED}ERROR: Program terminated with exit code $exit_code${NC}"
  failed_tests+=("Algorithm with time flag test - Exit code $exit_code")
else
  if echo "$output" | grep -q "Fibonacci Number 15 (decimal): 610" && echo "$output" | grep -q "Calculation Time:"; then
    echo -e "${GREEN}SUCCESS: Algorithm selection with other flags working correctly${NC}"
    ((passed_tests++))
  else
    echo -e "${RED}FAILED: Algorithm selection with other flags not working correctly${NC}"
    echo "  Expected: Fibonacci Number 15 (decimal): 610 and calculation time"
    echo "  Obtained: $output"
    failed_tests+=("Algorithm with time flag test - Incorrect output")
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
temp_output_file="/tmp/fib_test_all_flags_$$.txt"
./fib -a matrix -t -r -v -o "$temp_output_file" 15 2>/dev/null
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

echo -n "Testing missing argument for -a flag: "
if ! ./fib -a >/dev/null 2>&1; then
  echo -e "${GREEN}SUCCESS: Program correctly detected the error${NC}"
  ((passed_tests++))
else
  echo -e "${RED}FAILED: Program should have failed with missing -a argument${NC}"
  failed_tests+=("Missing -a argument - Did not fail as expected")
fi
((total_tests++))

echo -n "Testing missing argument for -f flag: "
if ! ./fib -f >/dev/null 2>&1; then
  echo -e "${GREEN}SUCCESS: Program correctly detected the error${NC}"
  ((passed_tests++))
else
  echo -e "${RED}FAILED: Program should have failed with missing -f argument${NC}"
  failed_tests+=("Missing -f argument - Did not fail as expected")
fi
((total_tests++))

echo -n "Testing invalid algorithm name: "
if ! ./fib -a invalid_algo 10 >/dev/null 2>&1; then
  echo -e "${GREEN}SUCCESS: Program correctly detected the error${NC}"
  ((passed_tests++))
else
  echo -e "${RED}FAILED: Program should have failed with invalid algorithm name${NC}"
  failed_tests+=("Invalid algorithm name - Did not fail as expected")
fi
((total_tests++))

echo -n "Testing invalid format name: "
if ! ./fib -f invalid_format 10 >/dev/null 2>&1; then
  echo -e "${GREEN}SUCCESS: Program correctly detected the error${NC}"
  ((passed_tests++))
else
  echo -e "${RED}FAILED: Program should have failed with invalid format name${NC}"
  failed_tests+=("Invalid format name - Did not fail as expected")
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

echo "Calculating Fibonacci(1000) with matrix algorithm..."
time ./fib -a matrix 1000 >/dev/null
perf_result=$?
((total_tests++))
if [ $perf_result -eq 0 ]; then
  ((passed_tests++))
else
  failed_tests+=("Matrix performance test - Failed with code $perf_result")
fi

echo -e "\n=== Stress test with time-only mode ==="
echo "Stress test: Fibonacci(100000) with time-only flag..."
output=$(./fib -T 100000)
perf_result=$?
((total_tests++))
if [ $perf_result -eq 0 ] && echo "$output" | grep -q "Calculation Time: "; then
  echo -e "${GREEN}SUCCESS: Stress test completed${NC}"
  echo "  $output"
  ((passed_tests++))
else
  echo -e "${RED}FAILED: Stress test failed${NC}"
  failed_tests+=("Stress test - Failed with code $perf_result")
fi

echo "Stress test: Fibonacci(500000) with matrix algorithm (time-only)..."
output=$(./fib -T -a matrix 500000)
perf_result=$?
((total_tests++))
if [ $perf_result -eq 0 ] && echo "$output" | grep -q "Calculation Time: "; then
  echo -e "${GREEN}SUCCESS: Matrix stress test completed${NC}"
  echo "  $output"
  ((passed_tests++))
else
  echo -e "${RED}FAILED: Matrix stress test failed${NC}"
  failed_tests+=("Matrix stress test - Failed with code $perf_result")
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
