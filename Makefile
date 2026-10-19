# Top-level Makefile for kukrimate/tpm-gpio-fail fork
# Builds tpm-gpio-detect (audit) and tpm-gpio-assert (execute)

.PHONY: all

all:
	$(MAKE) -C detect
	$(MAKE) -C reset
