export MAKEFLAGS += -rR --no-print-directory
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
export UTILDIR	    := $(BASEDIR)/util
export INCDIR	    := $(BASEDIR)/include

# Main Rule
SIMBENCH_APP	:= $(PLATOUTDIR)/simbench
HOST_APP	:= $(TOPOUTDIR)/host	

all: $(SIMBENCH_APP) $(HOST_APP)

# Components
SIMBENCH_DIRS		:= $(ARCHDIR) $(BMARKDIR) $(HARNESSDIR) $(PLATFORMDIR) $(UTILDIR)
SIMBENCH_BUILTINS	:= $(patsubst %, %/built-in.o, $(SIMBENCH_DIRS))
SIMBENCH_CLEAN_DIRS	:= $(patsubst %,__clean-%,$(SIMBENCH_DIRS))

# Build Configuration
include $(ARCHDIR)/Make.config
include $(PLATFORMDIR)/Make.simbench

%/built-in.o: .FORCE
	@make -f build/Makefile.build DIR=$(dir $@) __build

$(HOST_APP): $(TOPOUTDIR) .FORCE
	@make -C host

clean: $(SIMBENCH_CLEAN_DIRS) .FORCE
	-make -C host/ clean
	rm -rf out

$(SIMBENCH_CLEAN_DIRS): .FORCE
	@make -f build/Makefile.clean DIR=$(patsubst __clean-%,%,$@) __clean

$(ARCHOUTDIR): .FORCE
	@echo "  MKDIR   $(patsubst $(BASEDIR)/%,%,$@)"
	$(Q)mkdir -p $@

$(PLATOUTDIR): .FORCE
	@echo "  MKDIR   $(patsubst $(BASEDIR)/%,%,$@)"
	$(Q)mkdir -p $@

$(TOPOUTDIR): .FORCE
	@echo "  MKDIR   $(patsubst $(BASEDIR)/%,%,$@)"
	$(Q)mkdir -p $@

install : 
	 cp out/arm/ArndaleBoard/simbench /run/media/toky/BOOT/uImage &&  umount /dev/sdc* ; sync


.PHONY: .FORCE

