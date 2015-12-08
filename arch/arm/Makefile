include $(BASEDIR)/arch/arm/Make.config

ARCH_O := reset.o vector-handlers.o vectors.o arch.o mem.o memset.o memcpy.o

arch.a: $(ARCH_O)
	ar q $@ $^
	
clean:
	rm -f $(ARCH_O) arch.a
