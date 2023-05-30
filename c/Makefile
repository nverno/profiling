# Default optimization level
O     ?= -O2
TESTS ?= tst

# TODO: convert to header only library or compile to shared lib

all: tests
	@

tests: $(TESTS)

-include build/rules.mk


%.o: %.c profile.h $(BUILDSTAMP)
	$(call run,$(CC) $(CPPFLAGS) $(CFLAGS) $(O) $(DEPCFLAGS) -o $@ -c,COMPILE,$<)

$(TESTS): %: profile.o %.o
	$(call run,$(CC) $(CFLAGS) $(O) -o $@ $^ $(LDFLAGS) $(LIBS),LINK $@)


clean: clean-main
clean-main:
	$(call run,rm -f $(TESTS) *.o core *.core,CLEAN)
	$(call run,rm -rf $(DEPSDIR) files *.dSYM)
	$(RM) *.strace
distclean: clean

.PRECIOUS: %.o
.PHONY: all tests clean clean-main distclean
