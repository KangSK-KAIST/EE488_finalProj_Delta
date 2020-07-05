import m5
from m5.objects import *

root = Root()
root.full_system = False
root.system = System()

root.system.clk_domain = SrcClockDomain()
root.system.clk_domain.clock = '2GHz'
root.system.clk_domain.voltage_domain = VoltageDomain()

root.system.mem_mode = 'timing'
root.system.mem_ranges = [AddrRange('2GB')]
root.system.mem_ctrl = DDR3_1600_8x8()
root.system.mem_ctrl.range = root.system.mem_ranges[0]

root.system.cpu = TimingSimpleCPU()
root.system.membus = SystemXBar()

root.system.cpu.icache_port = root.system.membus.slave
root.system.cpu.dcache_port = root.system.membus.slave
root.system.mem_ctrl.port = root.system.membus.master
root.system.cpu.createInterruptController()
root.system.system_port = root.system.membus.slave
# root.system.cpu.interrupt[0].pio = root.system.membus.master
# root.system.cpu.interrupt[0].int_master = root.system.membus.slave
# root.system.cpu.interrupt[0].int_slave = root.system.membus.master

process = Process()
process.cmd = ['tests/test-progs/hello/bin/arm/linux/hello']
root.system.cpu.workload = process
root.system.cpu.createThreads()

m5.instantiate()
exit_event = m5.simulate()
print ('Existing @ tick', m5.curTick(), 'because', exit_event.getCause())
