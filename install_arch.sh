#!/bin/bash

# Install PATH
INS_PATH=$(pwd)

# Install packages
sudo pacman -Syu gcc scons zlib m4 swig protobuf gperftools python-pip python-virtualenv python-six python2

# Make python environment
cd $HOME/.pyenv || mkdir $HOME/.pyenv; cd $HOME/.pyenv
sudo rm -rf gem5
sudo virtualenv -p "/usr/bin/python2" gem5 &&
sudo chmod -R 0777 "$HOME/.pyenv/gem5" &&
source "gem5/bin/activate" &&

# Build
cd $INS_PATH/gem5
scons build/ARM/gem5.opt -j $(nproc) --ignore-style

# Run simulation
./build/ARM/gem5.opt --outdir="../results/LRU/1" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_1 --cache_LRU --thread_max=10000000
./build/ARM/gem5.opt --outdir="../results/LRU/2" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_2 --cache_LRU --thread_max=10000000
./build/ARM/gem5.opt --outdir="../results/LRU/3" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_3 --cache_LRU --thread_max=10000000
./build/ARM/gem5.opt --outdir="../results/LRU/4" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_4 --cache_LRU --thread_max=10000000
