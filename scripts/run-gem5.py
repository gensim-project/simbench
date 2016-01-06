
import optparse
import sys

import m5
from m5.defines import buildEnv
from m5.objects import *
from m5.util import addToPath, fatal

gem5_path='/home/harry/Work/gem5/gem5-stable'

addToPath(gem5_path+'/configs/common')
addToPath(gem5_path+'/configs/ruby')

from FSConfig import *
from SysPaths import *
from Benchmarks import *
import Simulation
import CacheConfig
import MemConfig
from Caches import *
import Options

from m5.objects import *
from Benchmarks import *
from m5.util import *

def makeArmSystem(mem_mode, machine_type, num_cpus=1,
                  dtb_filename=None, cmdline=None,
                  external_memory=""):
    assert machine_type
    self = ArmSystem()

    self.iobus = IOXBar()
    self.membus = MemBus()
    self.membus.badaddr_responder.warn_access = "warn"
    self.bridge = Bridge(delay='50ns')
    self.bridge.master = self.iobus.slave
    self.bridge.slave = self.membus.master

    self.mem_mode = mem_mode

    self.realview = RealViewPBX()

    #self.cf0 = CowIdeDisk(driveID='master')
    #self.cf0.childImage(mdesc.disk())

    # Attach any PCI devices this platform supports
    self.realview.attachPciDevices()
    # default to an IDE controller rather than a CF one
    #try:
    #    self.realview.ide.disks = [self.cf0]
    #except:
    #    self.realview.cf_ctrl.disks = [self.cf0]

    self.mem_ranges = []
    size_remain = 256 * 1024 * 1024
    warn("lol memory " + str(size_remain));
    for region in self.realview._mem_regions:
        if size_remain > long(region[1]):
            self.mem_ranges.append(AddrRange(region[0], size=region[1]))
            size_remain = size_remain - long(region[1])
        else:
            self.mem_ranges.append(AddrRange(region[0], size=size_remain))
            size_remain = 0
            break
        warn("Memory size specified spans more than one region. Creating" \
             " another memory controller for that range.")

    if size_remain > 0:
        fatal("The currently selected ARM platforms doesn't support" \
              " the amount of DRAM you've selected. Please try" \
              " another platform")

	# EOT character on UART will end the simulation
	self.realview.uart.end_on_eot = True

    if external_memory:
        # I/O traffic enters iobus
        self.external_io = ExternalMaster(port_data="external_io",
                                          port_type=external_memory)
        self.external_io.port = self.iobus.slave

        # Ensure iocache only receives traffic destined for (actual) memory.
        self.iocache = ExternalSlave(port_data="iocache",
                                     port_type=external_memory,
                                     addr_ranges=self.mem_ranges)
        self.iocache.port = self.iobus.master

        # Let system_port get to nvmem and nothing else.
        self.bridge.ranges = [self.realview.nvmem.range]

        self.realview.attachOnChipIO(self.iobus)
    else:
        self.realview.attachOnChipIO(self.membus, self.bridge)
    self.realview.attachIO(self.iobus)
    self.intrctrl = IntrControl()
    self.terminal = Terminal()
    self.vncserver = VncServer()

    self.system_port = self.membus.slave

    # Set the cache line size for the entire system
    self.cache_line_size = options.cacheline_size

    # Create a top-level voltage domain
    self.voltage_domain = VoltageDomain(voltage = options.sys_voltage)

    # Create a source clock for the system and set the clock period
    self.clk_domain = SrcClockDomain(clock =  options.sys_clock,
            voltage_domain = self.voltage_domain)

    # Create a CPU voltage domain
    self.cpu_voltage_domain = VoltageDomain()

    # Create a source clock for the CPUs and set the clock period
    self.cpu_clk_domain = SrcClockDomain(clock = options.cpu_clock,
                                             voltage_domain =
                                             self.cpu_voltage_domain)

    return self

def cmd_line_template():
    if options.command_line and options.command_line_file:
        print "Error: --command-line and --command-line-file are " \
              "mutually exclusive"
        sys.exit(1)
    if options.command_line:
        return options.command_line
    if options.command_line_file:
        return open(options.command_line_file).read().strip()
    return None


# Add options
parser = optparse.OptionParser()
Options.addCommonOptions(parser)
Options.addFSOptions(parser)

(options, args) = parser.parse_args()

(TestCPUClass, test_mem_mode, FutureClass) = Simulation.setCPUClass(options)

cmdline = cmd_line_template()
test_sys = makeArmSystem(test_mem_mode, options.machine_type,
						 options.num_cpus, options.dtb_filename,
						 cmdline=cmdline,
						 external_memory=options.external_memory_system)

# For now, assign all the CPUs to the same clock domain
test_sys.cpu = [TestCPUClass(clk_domain=test_sys.cpu_clk_domain, cpu_id=i)
				for i in xrange(1)]
				
# Sanity check
if options.fastmem:
	if TestCPUClass != AtomicSimpleCPU:
		fatal("Fastmem can only be used with atomic CPU!")
	if (options.caches or options.l2cache):
		fatal("You cannot use fastmem in combination with caches!")

for i in xrange(1):
	if options.fastmem:
		test_sys.cpu[i].fastmem = True
	if options.simpoint_profile:
		test_sys.cpu[i].addSimPointProbe(options.simpoint_interval)
	if options.checker:
		test_sys.cpu[i].addCheckerCpu()
	test_sys.cpu[i].createThreads()	

test_sys.kernel = './out/arm/realview-pba8/simbench'

CacheConfig.config_cache(options, test_sys)
MemConfig.config_mem(options, test_sys)

root = Root(full_system=True, system=test_sys)

Simulation.setWorkCountOptions(test_sys, options)
Simulation.run(options, root, test_sys, FutureClass)
