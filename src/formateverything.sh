#!/bin/bash 

# Grep all .cpp and .h files, and apply clang-format to themx
for file in `git ls-files | grep ".cpp\|.h"`;
do
    clang-format -i -style=file $file
done