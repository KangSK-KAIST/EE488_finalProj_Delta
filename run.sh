#!/bin/bash

cd gem5

./build/ARM/gem5.opt --outdir="../results/RWP/2MB/1" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_1 --l2_size=2MB --cache_RWP --thread_max=10000000 &
./build/ARM/gem5.opt --outdir="../results/RWP/2MB/2" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_2 --l2_size=2MB --cache_RWP --thread_max=10000000 &
./build/ARM/gem5.opt --outdir="../results/RWP/2MB/3" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_3 --l2_size=2MB --cache_RWP --thread_max=10000000 &
./build/ARM/gem5.opt --outdir="../results/RWP/2MB/4" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_4 --l2_size=2MB --cache_RWP --thread_max=10000000

./build/ARM/gem5.opt --outdir="../results/RWP/8MB/1" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_1 --l2_size=8MB --cache_RWP --thread_max=10000000 &
./build/ARM/gem5.opt --outdir="../results/RWP/8MB/2" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_2 --l2_size=8MB --cache_RWP --thread_max=10000000 &
./build/ARM/gem5.opt --outdir="../results/RWP/8MB/3" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_3 --l2_size=8MB --cache_RWP --thread_max=10000000 &
./build/ARM/gem5.opt --outdir="../results/RWP/8MB/4" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_4 --l2_size=8MB --cache_RWP --thread_max=10000000

./build/ARM/gem5.opt --outdir="../results/RWP/16MB/1" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_1 --l2_size=16MB --cache_RWP --thread_max=10000000 &
./build/ARM/gem5.opt --outdir="../results/RWP/16MB/2" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_2 --l2_size=16MB --cache_RWP --thread_max=10000000 &
./build/ARM/gem5.opt --outdir="../results/RWP/16MB/3" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_3 --l2_size=16MB --cache_RWP --thread_max=10000000 &
./build/ARM/gem5.opt --outdir="../results/RWP/16MB/4" configs/finalProj/two-level.py --cpu_clock=2GHz --bench_4 --l2_size=16MB --cache_RWP --thread_max=10000000
