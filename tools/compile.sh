#!/bin/bash

for dr in */; do
    cd "$dr"
    if [ -f Makefile ] || [ -f makefile ]; then
        make clean
        make
    else
        echo "Skipping '$dr' (no Makefile)"
    fi
    cd ..
done