#!/bin/bash

cd gem5

# Run simulation
./build/ARM/gem5.opt --outdir="../results/LRU/1" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_1 --cache_LRU --thread_max=100000
./build/ARM/gem5.opt --outdir="../results/LRU/2" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_2 --cache_LRU --thread_max=100000
./build/ARM/gem5.opt --outdir="../results/LRU/3" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_3 --cache_LRU --thread_max=100000
./build/ARM/gem5.opt --outdir="../results/LRU/4" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_4 --cache_LRU --thread_max=100000


./build/ARM/gem5.opt --outdir="../results/RWP/1" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_1 --cache_RWP --thread_max=100000
./build/ARM/gem5.opt --outdir="../results/RWP/2" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_2 --cache_RWP --thread_max=100000
./build/ARM/gem5.opt --outdir="../results/RWP/3" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_3 --cache_RWP --thread_max=100000
./build/ARM/gem5.opt --outdir="../results/RWP/4" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_4 --cache_RWP --thread_max=100000
