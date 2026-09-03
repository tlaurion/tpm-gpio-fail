# TPM GPIO Reset Attack PoC for Heads

Based on [kukrimate/tpm-gpio-fail](https://github.com/kukrimate/tpm-gpio-fail) by Mate Kukri.
<https://mkukri.xyz/2024/06/01/tpm-gpio-fail.html>

The `detect` tool is extended from kukri's `detect/detect.c`. The `assert` tool
is based on kukri's modified coreboot `reset/inteltool.c`. The top-level Makefile
builds them as `tpm-gpio-detect` and `tpm-gpio-assert` for Heads recovery shell.

`tpm-gpio-assert` calls `tpm2 shutdown -c` before assertion and `tpm2 startup -c`
after, following kukri's documented procedure. PCR values are shown before and
after assertion.

Built for Heads by `modules/tpm-gpio-reset`. Uses libpci, /dev/mem for PCR access.

## Usage (Heads recovery shell)

```bash
# Audit (safe, read-only)
tpm-gpio-detect 2>&1 | tee /media/tpm-gpio-detect.log

# Execute
tpm-gpio-assert 2>&1 | tee /media/tpm-gpio-assert.log
```

## Tested

NV4x ADL-P (0x5182): pads unlocked, NF1 mode confirmed, PCRs cleared to zero.

All other platform families are untested. Report results at:
<https://github.com/tlaurion/tpm-gpio-fail/issues>
