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
#Mount USB thumb drive read+write
mount-usb.sh --mode rw

# Audit (safe, read-only)
tpm-gpio-detect 2>&1 | tee /media/tpm-gpio-detect.log

# Execute
tpm-gpio-assert 2>&1 | tee /media/tpm-gpio-assert.log

#Unmount usb thumb drive
umount /media
```

## Exit codes

| Code | Meaning |
|---|---|
| 0 | DW0 write verified -- the pad reconfigured, PLTRST# edge created (vulnerable) |
| 1 | DW0 write ignored -- the pad is locked (or host-gated), no reset edge |
| 2 | platform not vulnerable by design (assertion skipped) |
| 126 | unknown PCH |
| 127 | hardware access error (not root, /dev/mem, P2SB) |

An exit 1 on a board that should be vulnerable is ambiguous between
"the firmware locked the pad" and "the host does not own the GPIO
community". Both tools print the discriminator: `HOST_OWN` (SKL/KBL
only, 0xAF:0xD4 bit 13; 1 = host owns the pad, 0 = SMM keeps ownership
and host writes are gated) and the `DW0 domain` (bits 31:30:
0=RSMRST/PWROK, 1=DEEP, 2=PLTRST, 3=unmapped/GLBRST).

## Verifying a firmware fix (two-ROM A/B)

The same pinned tool verifies that a coreboot-side PLTRST pad-lock fix
works: pre-test on the un-locked firmware, post-test on the fixed
firmware, same boards.

```bash
# 1. Boot the UN-LOCKED firmware (pre-test). In the recovery shell:
mount-usb.sh --mode rw
tpm-gpio-detect 2>&1 | tee /media/tpm-gpio-detect.log
flashrom -p internal -r - 2>/dev/null | sha256sum | tee /media/rom-hash.txt
tpm-gpio-assert 2>&1 | tee /media/tpm-gpio-assert.log   # expect exit 0
umount /media

# 2. Flash the FIXED firmware (pad lock enabled), repeat. Expect exit 1.
```

Expected: pre-test exit 0 (write verified, PCRs cleared), post-test
exit 1 (write ignored, pad locked). If the pre-test already exits 1,
check the HOST_OWN line before blaming the ROM: HOST_OWN = 0 means the
host does not own the GPIO community, so the pad reconfiguration is
gated regardless of the pad lock.

## Tested

NV4x ADL-P (0x5182): pads unlocked, NF1 mode confirmed, PCRs cleared to zero.

All other platform families are untested. Report results at:
<https://github.com/tlaurion/tpm-gpio-fail/issues>
