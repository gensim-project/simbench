OBJ :=

mem-benchmarks       := 
mem-benchmarks       += memory/tlb-flush.o
mem-benchmarks       += memory/tlb-evict.o
mem-benchmarks       += memory/memory-hot.o
mem-benchmarks       += memory/memory-cold.o
mem-benchmarks       += memory/memory-nonpriv.o

dispatch-benchmarks  := 
dispatch-benchmarks  += dispatch/same-page-indirect.o 
dispatch-benchmarks  += dispatch/inter-page-indirect.o
dispatch-benchmarks  += dispatch/same-page-direct.o 
dispatch-benchmarks  += dispatch/inter-page-direct.o
dispatch-benchmarks  += dispatch/inter-page-direct/ipd-fn-0.o dispatch/inter-page-direct/ipd-fn-1.o dispatch/inter-page-direct/ipd-fn-2.o dispatch/inter-page-direct/ipd-fn-3.o dispatch/inter-page-direct/ipd-fn-4.o dispatch/inter-page-direct/ipd-fn-5.o dispatch/inter-page-direct/ipd-fn-6.o dispatch/inter-page-direct/ipd-fn-7.o dispatch/inter-page-direct/ipd-fn-8.o dispatch/inter-page-direct/ipd-fn-9.o dispatch/inter-page-direct/ipd-fn-10.o 

codegen-benchmarks   := 
codegen-benchmarks   += codegen/small-blocks.o 
codegen-benchmarks   += codegen/large-blocks.o

exception-benchmarks := 
exception-benchmarks += exception/undef.o 
exception-benchmarks += exception/swi.o 
exception-benchmarks += exception/syscall.o 
exception-benchmarks += exception/dfault.o 
exception-benchmarks += exception/ifault.o

io-benchmarks        := 
io-benchmarks        += io/device-access.o 
io-benchmarks        += io/coprocessor.o

OBJ :=
OBJ += $(exception-benchmarks)
OBJ += $(codegen-benchmarks)
OBJ += $(dispatch-benchmarks)
OBJ += $(mem-benchmarks)
OBJ += $(io-benchmarks)
