LDLIBS = -lpci
tpm-gpio-assert: inteltool.o pcr.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
clean:
	rm -f *.o tpm-gpio-assert
