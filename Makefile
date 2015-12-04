ARCH ?= `uname -m`
PLATFORM ?= none

BENCHMARKS=
export BASEDIR=$(CURDIR)

all : out/$(ARCH)/$(PLATFORM)/simbench

PLATFORM_A = platform/$(ARCH)/$(PLATFORM)/platform.a
ARCH_A = arch/$(ARCH)/arch.a
HARNESS_A = out/$(ARCH)/harness.a
BMARKS_A = out/$(ARCH)/benchmarks.a

include platform/$(ARCH)/$(PLATFORM)/Make.simbench

$(ARCH_A) : 
	make -C arch/$(ARCH)

$(PLATFORM_A) : 
	make -C platform/$(ARCH)/$(PLATFORM)

$(HARNESS_A) :
	make -C harness/

$(BMARKS_A) : 
	make -C benchmarks/

clean:
	-make -C arch/$(ARCH) clean
	-make -C platform/$(ARCH)/$(PLATFORM) clean
	-make -C harness/ clean
	-make -C benchmarks/ clean
	rm -rf out
