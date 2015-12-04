include $(BASEDIR)/arch/$(ARCH)/Make.config

HARNESS_O := harness.o

harness.a : $(HARNESS_O)
	ar q $@ $^

clean:
	rm -f $(HARNESS_O) harness.a
