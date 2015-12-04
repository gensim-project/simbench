include $(BASEDIR)/arch/$(ARCH)/Make.config

HARNESS_O := harness.o
HARNESS_A := $(BASEDIR)/out/$(ARCH)/harness.a

$(HARNESS_A) : $(HARNESS_O)
	mkdir -p $(BASEDIR)/out/$(ARCH)
	ar q $@ $^

clean:
	rm -f $(HARNESS_O) $(HARNESS_A)
