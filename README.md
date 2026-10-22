# TPM GPIO Reset Attack PoC for Heads

Based on [kukrimate/tpm-gpio-fail](https://github.com/kukrimate/tpm-gpio-fail) by Mate Kukri.
<https://mkukri.xyz/2024/06/01/tpm-gpio-fail.html>

The `detect` tool is extended from kukri's `detect/detect.c`. The `assert` tool
is based on kukri's modified coreboot `reset/inteltool.c`. The top-level Makefile
builds them as `tpm-gpio-detect` and `tpm-gpio-assert` for Heads recovery shell.

On TPM2 hosts, `tpm-gpio-assert` calls `tpm2 shutdown -c` before assertion and
`tpm2 startup -c` after, following kukri's documented procedure. On TPM1.2
hosts it reads PCRs 0-15 via `tpm pcrread -ix` and re-initializes the chip
with a raw `TPM_Startup(TPM_FULL)` on /dev/tpm0 after the pulse. PCR values
are shown before and after assertion either way; the generation is picked
from which TPM CLI the initrd packed (`/bin/tpm2` vs `/bin/tpm`).

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
| 0 | PADCFGLOCK cleared (if set) and DW0 write verified -- PLTRST# edge created (vulnerable) |
| 1 | PADCFGLOCK re-locked by SMM, or DW0 write ignored -- no reset edge |
| 2 | platform not vulnerable by design (assertion skipped) |
| 126 | unknown PCH |
| 127 | hardware access error (not root, /dev/mem, P2SB) |

`tpm-gpio-assert` emulates a local root attacker: if PADCFGLOCK is set
it first clears the pad's lock bit (PADCFGLOCK is RW, per Intel 100
Series PCH Vol 2 25.1.10/25.1.12), re-reads it, and only then writes
DW0. A firmware lock only mitigates the attack if SMM re-locks the pad
after the clear attempt (reported as "re-locked"). `HOSTSW_OWN` (SKL/KBL
only, 0xAF:0xD4 bit 13) is *not* a write gate -- it selects whether GPIO
input events update GPI_STS (1) or the SMI/GPE status registers (0).
The `DW0 domain` line (bits 31:30: 0=RSMRST/PWROK, 1=DEEP, 2=PLTRST,
3=unmapped/GLBRST) is informational.

On emulated boards (QEMU i440fx `0x7010` / Q35 `0x2918` LPC bridges)
both tools name the emulated chipset and exit 126 -- expected, not an
error to report.

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

Expected: pre-test exit 0 (lock cleared, write verified, PCRs cleared),
post-test exit 1 (SMM re-locks the pad, write ignored). If the pre-test
already exits 1, read the `PADCFGLOCK Bypass` line before blaming the
ROM: `re-locked` means SMM enforced the lock even on the supposedly
unfixed firmware, so that build is already mitigated.

## Tested

NV4x ADL-P (0x5182): pads unlocked, NF1 mode confirmed, PCRs cleared to zero.

All other platform families are untested. Report results at:
<https://github.com/tlaurion/tpm-gpio-fail/issues>
