include $(BASEDIR)/arch/$(ARCH)/Make.config

benchmarks=memory/memory-hot.o memory/memory-cold.o


$(BASEDIR)/out/$(ARCH)/benchmarks.a : $(benchmarks)
	ar q $@ $^

clean:
	rm -f $(benchmarks)
