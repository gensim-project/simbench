-include $(BASEDIR)/Make.include

PLATFORM_O := uart.o platform.o mem.o

$(PLATFORM_A): $(PLATFORM_O)
	@echo "  AR      $(patsubst $(BASEDIR)/%,%,$@)"
	$(Q)$(AR) rc $@ $^

clean:
	rm -f $(PLATFORM_O) $(PLATFORM_A)
