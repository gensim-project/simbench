mem-benchmarks := memory/tlb-flush.o memory/tlb-evict.o memory/memory-hot.o memory/memory-cold.o 
dispatch-benchmarks := dispatch/same-page-indirect.o dispatch/inter-page-indirect.o dispatch/same-page-direct.o dispatch/inter-page-direct.o
codegen-benchmarks := codegen/small-blocks.o codegen/large-blocks.o

benchmarks :=
benchmarks += $(codegen-benchmarks)
benchmarks += $(dispatch-benchmarks)
benchmarks += $(mem-benchmarks)

OBJ := $(benchmarks)
