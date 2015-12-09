-include $(BASEDIR)/Make.include

HARNESS_O := harness.o

$(HARNESS_A): $(HARNESS_O)
	@echo "  AR      $(patsubst $(BASEDIR)/%,%,$@)"
	$(Q)$(AR) rc $@ $^

clean:
	rm -f $(HARNESS_O) $(HARNESS_A)
