import m5
from m5.objects import *
from new_cache import *

from optparse import OptionParser

# Parser
parser = OptionParser()

parser.add_option('--cpu_clock', type="string", default="2GHz")

parser.add_option('--ddr3_1600', action="store_true")
parser.add_option('--ddr3_2133', action="store_true")
parser.add_option('--ddr4_2400', action="store_true")
parser.add_option('--lpddr2_1066', action="store_true")
parser.add_option('--wideio_200', action="store_true")
parser.add_option('--lpddr3_1600', action="store_true")
parser.add_option('--gddr5_4000', action="store_true")

parser.add_option('--bench_1', action="store_true")
parser.add_option('--bench_2', action="store_true")
parser.add_option('--bench_3', action="store_true")
parser.add_option('--bench_4', action="store_true")

parser.add_option('--thread_max', type="int", default=1000000000)

parser.add_option('--cache_LRU', action="store_true")
parser.add_option('--cache_RWP', action="store_true")

parser.add_option('--l1_size', type="string", default="32kB")
parser.add_option('--l1_assoc', type="int", default=4)
parser.add_option('--l1_latency', type="int", default=2)
parser.add_option('--l2_size', type="string", default="2MB")
parser.add_option('--l2_assoc', type="int", default=8)
parser.add_option('--l2_latency', type="int", default=20)

(options, args) = parser.parse_args()


# Root Initialization
root = Root()
root.full_system = False
root.system = System()
#root = Root( full_system = False, system = System() )
root.system.clk_domain = SrcClockDomain()
root.system.clk_domain.clock = options.cpu_clock
root.system.clk_domain.voltage_domain = VoltageDomain()
root.system.mem_mode = 'timing'
root.system.mem_ranges = [AddrRange ('2048MB')]


# Memory type
if options.ddr3_1600:
	root.system.mem_ctrl = DDR3_1600_8x8()
elif options.ddr3_2133:
	root.system.mem_ctrl = DDR3_2133_8x8()
elif options.ddr4_2400:
	root.system.mem_ctrl = DDR4_2400_16x4()
elif options.lpddr2_1066:
	root.system.mem_ctrl = LPDDR2_S4_1066_1x32()
elif options.wideio_200:
	root.system.mem_ctrl = WideIO_200_1x128()
elif options.lpddr3_1600:
	root.system.mem_ctrl = LPDDR3_1600_1x32()
elif options.gddr5_4000:
	root.system.mem_ctrl = GDDR5_4000_2x32()
else:
	root.system.mem_ctrl = DDR3_1600_8x8()
root.system.mem_ctrl.range = root.system.mem_ranges[0]


# CPU type

root.system.cpu = DerivO3CPU()

root.system.cpu_clk_domain = SrcClockDomain()
root.system.cpu_clk_domain.clock = options.cpu_clock
root.system.cpu_clk_domain.voltage_domain = VoltageDomain()
root.system.cpu.clk_domain = root.system.cpu_clk_domain
# For x86 architecture
root.system.cpu.createInterruptController()
# root.system.cpu.interrupt[0].pio = root.system.membus.master
# root.system.cpu.interrupt[0].int_master = root.system.membus.slave
# root.system.cpu.interrupt[0].int_slave = root.system.membus.master


# Mem_ctrl system
root.system.membus = SystemXBar()
root.system.mem_ctrl.port = root.system.membus.master
root.system.system_port = root.system.membus.slave


# Default cache system
root.system.l1cache = L1Cache()
root.system.l2cache = L2Cache()

root.system.l1bus = L2XBar()

root.system.cpu.icache_port = root.system.l1bus.slave
root.system.cpu.dcache_port = root.system.l1bus.slave
root.system.l1cache.cpu_side = root.system.l1bus.master
root.system.l1cache.mem_side = root.system.l2cache.cpu_side
root.system.l2cache.mem_side = root.system.membus.slave

root.system.l1cache.replacement_policy = LRURP()
if options.cache_LRU:
    root.system.l2cache.replacement_policy = LRURP()
elif options.cache_RWP:
    root.system.l2cache.replacement_policy = RWPRP()
else:
    root.system.l2cache.replacement_policy = LRURP()

# Cache Options
root.system.l1cache.size = options.l1_size
root.system.l1cache.assoc = options.l1_assoc
root.system.l1cache.tag_latency = options.l1_latency
root.system.l2cache.size = options.l2_size
root.system.l2cache.assoc = options.l2_assoc
root.system.l2cache.tag_latency = options.l2_latency


# Process system
process = Process()
if options.bench_1:
	process.cmd = ['tests/2MM/2mm_base']
elif options.bench_2:
	process.cmd = ['tests/BFS/bfs','-f','test_bench/BFS/USA-road-d.NY.gr']
elif options.bench_3:
	process.cmd = ['tests/bzip2/bzip2_base.amd64-m64-gcc42-nn','test_bench/bzip2/input.source','280']
elif options.bench_4:
	process.cmd = ['tests/mcf/mcf_base.amd64-m64-gcc42-nn','test_bench/mcf/inp.in']
else:
	process.cmd = ['tests/test-progs/hello/bin/arm/linux/hello']

root.system.cpu.workload = process
root.system.cpu.createThreads()
root.system.cpu.max_insts_any_thread = options.thread_max
m5.instantiate()
exit_event = m5.simulate()
print('Existing @ tick ', m5.curTick(), ' because ', exit_event.getCause())
