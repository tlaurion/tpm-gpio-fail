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

## Register map

PLTRST# is GPP_B13 on every platform in scope. PADCFGLOCK lock bit for
PLTRST# = community-relative pad index 13, bit position `1<<13`.

| PCH | port | lock_offset | lock_bit | source |
|-----|------|-------------|----------|--------|
| SKL/KBL S/H (PCH-H) | 0xaf | 0x98 | 1<<13 | coreboot gerrit 95661 (PCH-H base 0x90) + Linux SPT_H_PADCFGLOCK |
| SKL/KBL LP (PCH-LP) | 0xaf | 0xa8 | 1<<13 | Linux SPT_LP_PADCFGLOCK (0xa0) + kukri 21c394e |
| ADL-P/U/N | 0x6e | 0x80 | 1<<13 | Intel doc 834810 Table 2 |
| ADL-S | 0x6d | 0x110 | 1<<13 | Intel doc 834810 Table 3 |
| RPL-P | 0x6e | 0x80 | 1<<13 | coreboot RPL-P; see note (doc 834810 Table 2 lists 0x6d/0x110) |
| RPL-S | 0x6d | 0x110 | 1<<13 | Intel doc 834810 Table 3 |
| Arrow Lake S | 0x6d | 0x120 | 1<<13 | Intel doc 834810 Table 3 |
| CFL | 0x6e | 0x88 | 1<<13 | kukri 21c394e (not listed in 834810) |
| CML-U/Y | 0x6e | 0x88 | 1<<13 | Intel doc 834810 Table 2 |
| CML-DT/H | 0x6e | 0x88 | 1<<13 | Intel doc 834810 Table 3; see note (B460/H410 listed 0xaf/0xa8) |
| CNP-LP | 0x6e | 0x88 | 1<<13 | coreboot cannonlake (not listed in 834810) |
| EHL | 0x6e | 0x80 | 1<<13 | coreboot elkhartlake (flagged inferred) |
| JSL | 0x6e | 0x90 | 1<<13 | coreboot jasperlake gpio_defs.h (not listed in 834810) |
| GLK | 0xc5 | 0x80 | 1<<22 | coreboot apollolake gpio_glk.h (not listed in 834810) |
| MTL | n/a | - | - | no PLTRST# pad definition (untested) |
| Pre-SKL | n/a | - | - | no PLTRST# pad definition (untested) |

Intel doc 834810 ("General Purpose I/O Configuration Best Practices", ID
834810, public) directly lists the PLTRST# PADCFGLOCK offset/bit only for
ADL-P/U/N, TGL, CML-U, ADL-S/RPL-S, CML-DT (400-series) and ARL-S. Every
other row above is sourced from coreboot source, Linux, or kukri and is
NOT confirmed by doc 834810; the EHL row is explicitly inferred.

Two doc-834810 rows conflict with the per-platform values in this tree and
are flagged rather than silently matched:

- RPL-P: doc 834810 Table 2 groups Raptor Lake P with RPL-HX/H/U at
  0x6D/0x110, but `rpl_p.c` uses 0x6e/0x80 (same GPIO IP as ADL-P).
- CML-DT: doc 834810 Table 3 lists the B460/H410 variant at 0xAF/0xA8,
  but `cml_dt.c` uses 0x6e/0x88 for all CML-DT/H.

Reference: Intel doc 834810 = "General Purpose I/O Configuration Best Practices",
ID 834810 (public): https://www.intel.com/content/www/us/en/developer/articles/technical/software-security-guidance/technical-documentation/gpio-configuration-best-practices.html

LPC/eSPI pin lock offsets from `detect/platforms/*.c` espi pin tables:
SKL/KBL S/H 0x90 (port 0xaf), SKL/KBL LP 0xa0 (port 0xaf); ADL-P/N, CFL,
CML-U/Y &amp; CML-DT/H, CNP-LP, TGL 0x80 (port 0x6e); ADL-S/RPL-S 0x110 (port
0x6d); EHL 0x90 (port 0x6b); JSL 0x98 (port 0x6e); GLK 0x90 (port 0xc5).
No single group-0 base + 8 formula holds across families.

## Usage (Heads recovery shell)

### Pre-test (un-locked firmware ROM)

```bash
mount-usb.sh --mode rw && tpm-gpio-detect 2>&1 | tee /media/pre-detect.log ; umount /media
mount-usb.sh --mode rw && tpm-gpio-assert 2>&1 | tee /media/pre-assert.log ; umount /media
mount-usb.sh --mode rw && ( echo "---- BEFORE ----" && $CONFIG_FLASH_OPTIONS 2>&1 ) | tee /media/pre-lock.log ; umount /media
mount-usb.sh --mode rw && cbmem --console 2>&1 | tee /media/pre-cbmem.log ; umount /media
```

Rename or move these `pre-*` logs aside, then flash the fixed firmware ROM.

### Post-test (fixed firmware ROM)

```bash
mount-usb.sh --mode rw && tpm-gpio-detect 2>&1 | tee /media/post-detect.log ; umount /media
mount-usb.sh --mode rw && tpm-gpio-assert 2>&1 | tee /media/post-assert.log ; umount /media
mount-usb.sh --mode rw && ( echo "---- BEFORE ----" && $CONFIG_FLASH_OPTIONS 2>&1 ) | tee /media/post-lock.log ; umount /media
mount-usb.sh --mode rw && cbmem --console 2>&1 | tee /media/post-cbmem.log ; umount /media
```

`tpm-gpio-detect` audits GPIO lock status (read-only).
`tpm-gpio-assert` attempts to reconfigure PLTRST#; the effect only
persists until the next platform reboot -- it is not destructive, and
its result is not meant to survive one.
`cbmem --console` dumps the coreboot console buffer; on boards whose
SMM logs pad-lock activity you may see messages such as
`gpio_lock_smi: SoC GPIO pads locked` there -- advisory only, never a
pass/fail criterion.

## Exit codes

| Code | Meaning |
|------|---------|
| 0 | DW0 write verified -- the pad reconfigured, PLTRST# edge created (vulnerable) |
| 1 | DW0 write ignored -- the pad is locked (or host-gated), no reset edge |
| 2 | platform untested -- no PLTRST# pad definition (assertion skipped) |
| 126 | unknown PCH |
| 127 | hardware access error (not root, /dev/mem, P2SB) |

An exit 1 on a board that should be vulnerable is ambiguous between
"the firmware locked the pad" and "the host does not own the GPIO
community". Both tools print the discriminator: `HOST_OWN` (SKL/KBL
only, 0xAF:0xD4 bit 13; 1 = host owns the pad, 0 = SMM keeps ownership
and host writes are gated) and the `DW0 domain` (bits 31:30:
0=RSMRST/PWROK, 1=DEEP, 2=PLTRST, 3=unmapped/GLBRST).

On emulated boards (QEMU i440fx `0x7010` / Q35 `0x2918` LPC bridges)
`detect` names the emulated bridge explicitly and exits 126; `assert`
reports an unknown PCH device ID and also exits 126 -- expected, not an
error to report.

## Verifying a firmware fix (two-ROM A/B)

Run the Pre-test Usage commands on the un-locked firmware, rename or
move the `pre-*` logs aside, flash the fixed firmware ROM, then run
the Post-test Usage commands and compare against the verdict below.

**Verdict** (never the exit code alone):

- **Detect**: PADCFGLOCK bits 0 (pre) → set (post)
- **Assert**: exit 0 (pre, write verified, PCRs cleared) → exit 1 (post, write ignored, pad locked), PCRs unchanged
- **Lock**: flashprog PR0 unlocked (pre) → locked (post)
- **cbmem --console**: advisory only; SMM pad-lock messages appear on some boards, not all

If the pre-test already exits 1, check the HOST_OWN line before blaming
the ROM: HOST_OWN = 0 means the host does not own the GPIO community, so
the pad reconfiguration is gated regardless of the pad lock.

## Tested

NV4x ADL-P (0x5182): verified per linuxboot/heads PR 2227 — pre-lock ROM
reports pads unlocked (vulnerable); after the pad-lock SMI,
`tpm-gpio-detect` reports `GPP_B13 PLTRST# 0x00002000 (LOCKED)`, verdict
`MITIGATED`, and `tpm-gpio-assert` reports `DW0 write ignored --
PADCFGLOCK set` with PCRs unchanged; `cbmem --console` shows
`gpio_lock_smi: SoC GPIO pads locked (1 pad)` (advisory confirmation of
the lock SMI, not a pass/fail criterion).

All other platform families are untested. Report results at:
<https://github.com/tlaurion/tpm-gpio-fail/issues>
