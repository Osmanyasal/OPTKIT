#!/bin/bash

source ./uninstall.sh

for dr in $(ls -d */);
do
    cd $dr
    make clean
    make
    sudo cp optkit_* /usr/local/bin
    cd .. 
done    

sudo cp optkit_sandbox_utils.py /usr/local/include
cd governor_model; ./optkit_freq_model_generator; cd ..
sudo cp -r ./governor_model/saved_model /usr/local/include/optkit_governor_model