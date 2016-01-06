mem-benchmarks       := memory/tlb-flush.o memory/tlb-evict.o memory/memory-hot.o memory/memory-cold.o
mem-benchmarks       += memory/memory-nonpriv.o
dispatch-benchmarks  := dispatch/same-page-indirect.o dispatch/inter-page-indirect.o
dispatch-benchmarks  += dispatch/same-page-direct.o dispatch/inter-page-direct.o
codegen-benchmarks   := codegen/small-blocks.o codegen/large-blocks.o
exception-benchmarks := exception/dfault.o exception/ifault.o
io-benchmarks        := io/device-access.o

OBJ :=
OBJ += $(exception-benchmarks)
OBJ += $(codegen-benchmarks)
OBJ += $(dispatch-benchmarks)
OBJ += $(mem-benchmarks)
OBJ += $(io-benchmarks)
