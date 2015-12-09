export MAKEFLAGS += -rR --no-print-directory -s
export Q := @
	
# Architecture/Platform Selection
export ARCH	?= $(shell uname -m)
export PLATFORM	?= none

# Directory Configuration
export BENCHMARKS   :=
export BASEDIR	    := $(CURDIR)
export PLATFORMDIR  := $(BASEDIR)/platform/$(ARCH)/$(PLATFORM)
export ARCHDIR	    := $(BASEDIR)/arch/$(ARCH)
export TOPOUTDIR    := $(BASEDIR)/out
export ARCHOUTDIR   := $(TOPOUTDIR)/$(ARCH)
export PLATOUTDIR   := $(ARCHOUTDIR)/$(PLATFORM)
export HARNESSDIR   := $(BASEDIR)/harness
export BMARKDIR	    := $(BASEDIR)/benchmarks
export INCDIR	    := $(BASEDIR)/include
	
# Main Rule
SIMBENCH_APP	:= $(PLATOUTDIR)/simbench
HOST_APP	:= $(TOPOUTDIR)/host	

all: $(SIMBENCH_APP) $(HOST_APP)
	
# Components
export PLATFORM_A   := $(PLATOUTDIR)/platform.a
export ARCH_A	    := $(ARCHOUTDIR)/arch.a
export HARNESS_A    := $(ARCHOUTDIR)/harness.a
export BMARKS_A	    := $(ARCHOUTDIR)/benchmarks.a

# Build Configuration
-include $(ARCHDIR)/Make.config
-include $(PLATFORMDIR)/Make.simbench

$(ARCH_A): $(ARCHOUTDIR) .FORCE
	@make -C $(ARCHDIR)

$(PLATFORM_A): $(PLATOUTDIR) .FORCE
	@make -C $(PLATFORMDIR)

$(HARNESS_A): $(ARCHOUTDIR) .FORCE
	@make -C $(HARNESSDIR)

$(BMARKS_A): $(ARCHOUTDIR) .FORCE
	@make -C $(BMARKDIR)

$(HOST_APP): $(TOPOUTDIR) .FORCE
	@make -C host

clean:
	-make -C $(ARCHDIR) clean
	-make -C $(PLATFORMDIR) clean
	-make -C $(HARNESSDIR) clean
	-make -C $(BMARKDIR) clean
	-make -C host/ clean
	rm -rf out
	
$(ARCHOUTDIR): .FORCE
	@echo "  MKDIR   $(patsubst $(BASEDIR)/%,%,$@)"
	$(Q)mkdir -p $@

$(PLATOUTDIR): .FORCE
	@echo "  MKDIR   $(patsubst $(BASEDIR)/%,%,$@)"
	$(Q)mkdir -p $@

$(TOPOUTDIR): .FORCE
	@echo "  MKDIR   $(patsubst $(BASEDIR)/%,%,$@)"
	$(Q)mkdir -p $@

.PHONY: .FORCE
