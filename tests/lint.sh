#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

failed_checks=()
TMPFILE=$(mktemp)
trap 'rm -f $TMPFILE' EXIT

cd ..

echo -e "${YELLOW}=== Source Code Analysis ===${NC}"

echo -e "\n${YELLOW}Checking shell scripts with shellcheck...${NC}"
while IFS= read -r -d '' file; do
  echo -n "Analyzing $file: "
  if shellcheck "$file"; then
    echo -e "${GREEN}OK${NC}"
  else
    echo -e "${RED}ERROR${NC}"
    echo "ShellCheck: $file" >> "$TMPFILE"
  fi
done < <(find . -name "*.sh" -type f -print0)

echo -e "\n${YELLOW}Checking C code formatting...${NC}"
while IFS= read -r -d '' file; do
  echo -n "Analyzing format in $file: "
  if clang-format --dry-run --Werror "$file" 2>/dev/null; then
    echo -e "${GREEN}OK${NC}"
  else
    echo -e "${RED}FORMAT ERROR${NC}"
    clang-format --output-replacements-xml "$file" | grep -v "<?xml" | grep "<replacement " | head -3
    echo "Format: $file" >> "$TMPFILE"
  fi
done < <(find . \( -name "*.c" -o -name "*.h" \) -type f -print0)

while IFS= read -r line; do
  failed_checks+=("$line")
done < "$TMPFILE"

echo -e "\n${YELLOW}=== Verification Summary ===${NC}"
echo -e "Total checks with issues: ${#failed_checks[@]}"

if [ ${#failed_checks[@]} -gt 0 ]; then
  echo -e "\n${RED}Problems found:${NC}"
  for ((i=0; i<${#failed_checks[@]}; i++)); do
    echo -e "  ${RED}$((i+1)). ${failed_checks[$i]}${NC}"
  done
  echo -e "\n${RED}VERIFICATION FAILED${NC}"
  exit 1
else
  echo -e "${GREEN}ALL CHECKS PASSED SUCCESSFULLY${NC}"
  exit 0
fi
