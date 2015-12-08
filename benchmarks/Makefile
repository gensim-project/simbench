include $(BASEDIR)/arch/$(ARCH)/Make.config

mem-benchmarks=memory/tlb-flush.o memory/tlb-evict.o #memory/memory-hot.o memory/memory-cold.o 

benchmarks=$(mem-benchmarks)


$(BASEDIR)/out/$(ARCH)/benchmarks.a : $(benchmarks)
	ar q $@ $^

clean:
	rm -f $(benchmarks)
