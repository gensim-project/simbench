mem-benchmarks	    := memory/tlb-flush.o memory/tlb-evict.o memory/memory-hot.o memory/memory-cold.o
mem-benchmarks	    += memory/memory-nonpriv.o
dispatch-benchmarks := dispatch/same-page-indirect.o dispatch/inter-page-indirect.o dispatch/same-page-direct.o
dispatch-benchmarks += dispatch/inter-page-direct.o
codegen-benchmarks  := codegen/small-blocks.o codegen/large-blocks.o

OBJ :=
OBJ += $(codegen-benchmarks)
OBJ += $(dispatch-benchmarks)
OBJ += $(mem-benchmarks)
