-include $(BASEDIR)/Make.include

ARCH_O := reset.o vector-handlers.o vectors.o arch.o mem.o memset.o memcpy.o

$(ARCH_A): $(ARCH_O)
	@echo "  AR      $(patsubst $(BASEDIR)/%,%,$@)"
	$(Q)$(AR) rc $@ $^
	
clean:
	rm -f $(ARCH_O) $(ARCH_A)
