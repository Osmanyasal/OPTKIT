#!/bin/bash

if [[ "$1" == "help" || "$1" == "--help" || "$1" == "-help" ]]; 
then
    echo "This script changes integers with cstdint integers in all *.cc files inside ./src/** "
    exit 0
fi

EXCLUDE_DIR="src/core/events"

# Use find to generate a list of .cc files
cc_files_list=$(find ./src/ -type f \( -name "*.cc" -o -name "*.hh" \) ! -path "$EXCLUDE_DIR/*")

# Iterate through the list of .cc files and process each file
for file in $cc_files_list; do

    if [ "$file" == "./src/utils/json.hh" ]; then
        echo "Skipping file: $file"
        continue
    fi
    # Perform replacements using sed and save to a temporary file
    sed -E 's/\bunsigned[[:space:]]+int\b/uint32_t/g;
            s/\bunsigned[[:space:]]+char\b/uint8_t/g;
            s/\bunsigned[[:space:]]+short\b/uint16_t/g;
            s/\bunsigned[[:space:]]+long\b/uint64_t/g;
            s/\bshort[[:space:]]+int\b/int16_t/g;
            s/\bint\b/int32_t/g;
            s/\bdouble\b/double/g;
            s/\bfloat\b/float/g;
            s/\bchar\b/char/g;
            s/\bshort\b/int16_t/g;
            s/\blong\b/int64_t/g;' "$file" > "$file.tmp"
    
    # Overwrite the original file with the temporary file
    mv "$file.tmp" "$file"
done


# Find all .hh and .cc files
find "./src" -type f \( -name "*.hh" -o -name "*.cc" \) ! -path "$EXCLUDE_DIR/*" | while read file; do

    if [ "$file" == "./src/utils/json.hh" ]; then
        echo "Skipping file: $file"
        continue
    fi

    echo "Processing file: $file"

    # Save the original file permissions
    original_permissions=$(stat -c "%a" "$file")

    # Create a temporary file to store the modified content
    temp_file=$(mktemp)

    # Use sed to remove "src/" prefix in #include statements, using "|" as the delimiter
    sed 's|\(#include "\)src/\(.*"\)|\1\2|' "$file" > "$temp_file"

    # Move the modified content back to the original file
    mv "$temp_file" "$file"

    # Restore the original file permissions
    chmod "$original_permissions" "$file"

    echo "Updated file: $file"
done

echo "Done processing all files."
