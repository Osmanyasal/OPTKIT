#!/bin/bash

CSPDLOG="Custom SPDLOG"

if [ $# -gt 0 ]; then
    if [ "$1" = "--help" ]; then
        echo "$0 [clean]"
        exit 0
    fi
    if [ "$1" = "clean" ]; then
        echo "${CSPDLOG} executing make clean"
        rm -rf ./build
        exit 0
    else
        echo "Unknown parameter: $1"
        exit 1
    fi
fi

git checkout v1.x
mkdir -p build
echo "${CSPDLOG} build folder created"
cd build
cmake .. && make -j
echo "${CSPDLOG} cmake executed" 
cd ..

#sudo find ./ -type f -name "*.h*" -exec cp {} "/usr/local/include/" \;
sudo cp -R ./include/spdlog /usr/local/include/
sudo cp ./build/libspdlog.a /usr/local/lib
