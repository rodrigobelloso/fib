#!/bin/bash
# Generate a random 6-character build ID
# Uses alphanumeric characters (0-9, a-z)

BUILD_ID=$(LC_ALL=C tr -dc 'a-z0-9' < /dev/urandom | head -c 6)
echo "#ifndef BUILD_ID_H"
echo "#define BUILD_ID_H"
echo "#define BUILD_ID \"$BUILD_ID\""
echo "#endif"
