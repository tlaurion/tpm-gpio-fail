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

## Register map (verified against Intel doc 834810 and kukri `21c394e`)

PLTRST# is GPP_B13 on every platform in scope. PADCFGLOCK lock bit for
PLTRST# = community-relative pad index 13, bit position `1<<13`.

| PCH | port | lock_offset | lock_bit | source |
|-----|------|-------------|----------|--------|
| SKL/KBL S/H (PCH-H) | 0xaf | 0xa8 | 1<<13 | Intel 332691/335193 + kukri |
| SKL/KBL LP (PCH-LP) | 0xaf | 0xa8 | 1<<13 | Intel 332691/335193 + kukri |
| ADL-P/U/N | 0x6e | 0x80 | 1<<13 | Intel doc 834810 Table 2 |
| ADL-S | 0x6d | 0x110 | 1<<13 | Intel doc 834810 Table 3 |
| RPL-P | 0x6e | 0x80 | 1<<13 | same arch as ADL-P |
| RPL-S | 0x6d | 0x110 | 1<<13 | Intel doc 834810 Table 3 |
| CFL | 0x6e | 0x88 | 1<<13 | kukri 21c394e |
| CML-U/Y | 0x6e | 0x88 | 1<<13 | Intel doc 834810 Table 2 |
| CML-DT/H | 0x6e | 0x88 | 1<<13 | Intel doc 834810 Table 3 |
| CNP-LP | 0x6e | 0x88 | 1<<13 | CML 400-series GPIO IP |
| EHL | 0x6e | 0x80 | 1<<13 | coreboot elkhartlake (flagged inferred) |
| JSL | 0x6e | 0x90 | 1<<13 | coreboot jasperlake gpio_defs.h |
| GLK | 0xc5 | 0x80 | 1<<22 | coreboot apollolake gpio_glk.h |
| MTL | not vulnerable | - | - | functional GPIO lock |
| Pre-SKL | not vulnerable | - | - | dedicated PLTRST# pin |

LPC/eSPI pins (GPP_A1–A9) use lock_offset 0xa0 on SKL/KBL, 0x80 on CML/CFL/CNP,
0x90 on ADL-S/RPL-S — i.e. lock_offset = base + 1*8 where base is the GPP_A
group-0 lock register.

## Usage (Heads recovery shell)

```bash
# Mount USB thumb drive read+write
mount-usb.sh --mode rw

# Audit (safe, read-only)
tpm-gpio-detect 2>&1 | tee /media/tpm-gpio-detect.log

# Execute
tpm-gpio-assert 2>&1 | tee /media/tpm-gpio-assert.log

# Unmount usb thumb drive
umount /media
```

## Exit codes

| Code | Meaning |
|------|---------|
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

Expected: pre-test exit 0 (write verified, PCRs cleared), post-test
exit 1 (write ignored, pad locked). If the pre-test already exits 1,
check the HOST_OWN line before blaming the ROM: HOST_OWN = 0 means the
host does not own the GPIO community, so the pad reconfiguration is
gated regardless of the pad lock.

## Tested

NV4x ADL-P (0x5182): pads unlocked, NF1 mode confirmed, PCRs cleared to zero.

All other platform families are untested. Report results at:
<https://github.com/tlaurion/tpm-gpio-fail/issues>
