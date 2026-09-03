// SPDX-License-Identifier: GPL-2.0-only
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <pci/pci.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/io.h>
#include <unistd.h>
#include "platforms/defs.h"

#ifndef PCI_BASE_ADDRESS_0
#define PCI_BASE_ADDRESS_0	0x10
#endif

extern struct platform platform_skl_kbl_s_h;
extern struct platform platform_skl_kbl_lp;
extern struct platform platform_cfl_s_h;
extern struct platform platform_adl_p;
extern struct platform platform_adl_s;
extern struct platform platform_rpl_s;
extern struct platform platform_rpl_p;
extern struct platform platform_cnp_lp;
extern struct platform platform_cml_u;
extern struct platform platform_cml_dt;
extern struct platform platform_tgl;
extern struct platform platform_arl_s;
extern struct platform platform_mtl;
extern struct platform platform_pre_skl;

static struct platform *platforms[] = {
	&platform_skl_kbl_s_h,
	&platform_skl_kbl_lp,
	&platform_cfl_s_h,
	&platform_adl_p,
	&platform_adl_s,
	&platform_rpl_s,
	&platform_rpl_p,
	&platform_cnp_lp,
	&platform_cml_u,
	&platform_cml_dt,
	&platform_tgl,
	&platform_arl_s,
	&platform_mtl,
	&platform_pre_skl,
	NULL
};

static int devmemfd = -1;
static uint64_t sbreg_base = 0;

__attribute__((destructor)) static void close_devmemfd(void)
{
	if (devmemfd != -1)
		close(devmemfd);
}

static uint32_t pcr_read32(uint8_t port, uint16_t offset)
{
	uint32_t ret;
	if (pread(devmemfd, &ret, sizeof ret, sbreg_base + (port << 16) + offset) != sizeof ret) {
		perror("/dev/mem");
		ret = -1;
	}
	return ret;
}

static void pcr_write32(uint8_t port, uint16_t offset, uint32_t val)
{
	if (pwrite(devmemfd, &val, sizeof val, sbreg_base + (port << 16) + offset) != sizeof val)
		perror("/dev/mem");
}

static int detect_pinset(struct pin *pins)
{
	int unlocked = 0;
	for (struct pin *pin = pins; pin->gpio; ++pin) {
		uint32_t lock_val = pcr_read32(pin->port, pin->lock_offset);
		uint32_t locktx_val = pcr_read32(pin->port, pin->lock_offset + 4);
		int cfg_locked = !!(lock_val & pin->lock_bit);
		int tx_locked = !!(locktx_val & pin->lock_bit);
		unlocked += !(cfg_locked && tx_locked);

		char cfg_str[32], tx_str[32];
		snprintf(cfg_str, sizeof(cfg_str), "0x%08x (%s)", lock_val,
			 cfg_locked ? "LOCKED" : "UNLOCKED");
		snprintf(tx_str, sizeof(tx_str), "0x%08x (%s)", locktx_val,
			 tx_locked ? "LOCKED" : "UNLOCKED");
		printf("  %-10s %-12s %-22s %-22s",
		       pin->gpio, pin->nf, cfg_str, tx_str);
		if (!cfg_locked && !tx_locked)
			printf("  <- UNLOCKED");
		printf("\n");
	}
	return unlocked;
}

static int scan_platform(struct platform *platform)
{
	int status = 0;
	if (platform->global_pins) {
		printf("\n--- PLTRST# Pad Lock Status ---\n");
		printf("  %-10s %-12s %-22s %-22s\n",
		       "GPIO", "Signal", "PADCFGLOCK", "PADCFGLOCKTX");
		status = detect_pinset(platform->global_pins);
	} else {
		printf("No PLTRST# pad definition for this platform (not affected)\n");
	}

	/* Read DW0 to check pad mode */
	if (platform->global_pins && platform->pad_cfg_base > 0) {
		uint32_t dw0_offs = platform->pad_cfg_base +
			(__builtin_ctz(platform->global_pins->lock_bit) * platform->pad_stride);
		uint32_t dw0 = pcr_read32(platform->global_pins->port, dw0_offs);
		uint32_t pad_mode = (dw0 >> 10) & 0xf;
		const char *mode_str = pad_mode == 0 ? "GPIO" : pad_mode == 1 ? "NF1" : "NF2+";
		printf("\n--- Pad Configuration ---\n");
		printf("DW0 (0x%04x) = 0x%08x, mode: %s (bits 13:10=0x%x)\n",
		       dw0_offs, dw0, mode_str, pad_mode);
		if (pad_mode == 1)
			printf("NF1 mode -- mode transition CAN create PLTRST# reset edge\n");
		else if (pad_mode == 0)
			printf("GPIO mode -- mode locked, transition NOT possible\n");
	}

	int is_espi = 1;
	if (platform->lpc_pins)
		is_espi = !!(pcr_read32(platform->espi_check_port, platform->espi_check_offset) & platform->espi_check_bit);

	if (platform->espi_pins || platform->lpc_pins) {
		printf("\n--- %s Bus Pin Lock Status ---\n",
		       is_espi ? "eSPI" : "LPC");
		printf("  %-10s %-12s %-22s %-22s\n",
		       "GPIO", "Signal", "PADCFGLOCK", "PADCFGLOCKTX");
		if (is_espi)
			status += detect_pinset(platform->espi_pins);
		else
			status += detect_pinset(platform->lpc_pins);
	} else {
		printf("No bus pins defined for this platform\n");
	}

	/* Vulnerability classification */
	printf("\n--- Vulnerability Classification ---\n");
	if (platform->tier == -1) {
		printf("NOT VULNERABLE: %s\n", platform->summary);
	} else if (platform->tier == 1) {
		printf("TIER 1 -- CONFIRMED VULNERABLE: %s\n", platform->summary);
		printf("Attack confirmed working by community testing.\n");
	} else if (platform->tier == 2) {
		printf("TIER 2 -- VULNERABLE (UNCONFIRMED): %s\n", platform->summary);
		printf("PADCFGLOCK absent per Intel 834810 but no community test data.\n");
		printf("Run tpm-gpio-assert to test.\n");
	} else if (platform->tier == 3) {
		printf("TIER 3 -- VULNERABILITY UNCERTAIN: %s\n", platform->summary);
		printf("Pad may be reconfigured but attack not confirmed on this PCH die.\n");
		printf("Run tpm-gpio-assert to test.\n");
	}

	return status;
}

int main(void)
{
	setbuf(stdout, NULL);

	if (iopl(3)) {
		fprintf(stderr, "iopl(3) failed: %s\n", strerror(errno));
		printf("\n=== RESULT: hardware access error ===\n");
		return 127;
	}

	struct pci_access *pacc = pci_alloc();
	pacc->method = PCI_ACCESS_I386_TYPE1;
	pci_init(pacc);
	pci_scan_bus(pacc);

	/* 1. Find ISA/LPC bridge by device class */
	struct pci_dev *isa = NULL;
	for (struct pci_dev *dev = pacc->devices; dev; dev = dev->next) {
		pci_fill_info(dev, PCI_FILL_CLASS);
		if (dev->vendor_id == 0x8086
		    && dev->device_class == 0x0601) {
			isa = dev;
			break;
		}
	}
	if (!isa) {
		fprintf(stderr, "No Intel ISA/LPC bridge found (device class 0x0601) -- cannot access PCI\n");
		pci_cleanup(pacc);
		printf("\n=== RESULT: hardware access error ===\n");
		return 127;
	}

	/* 2. Match platform by ISA device ID */
	uint16_t isa_device = isa->device_id;
	struct platform *platform = NULL;
	for (struct platform **pp = platforms; *pp; ++pp) {
		for (struct pch *pch = (*pp)->pchs; pch->name; ++pch) {
			if (pch->pid == isa_device) {
				printf("Found platform %s with PCH %s!\n", (*pp)->name, pch->name);
				platform = *pp;
				break;
			}
		}
	}
	if (!platform) {
		fprintf(stderr, "Unknown ISA bridge device %04x -- not in platform database\n", isa_device);
		pci_cleanup(pacc);
		printf("\n=== RESULT: unknown platform ===\n");
		return 126;
	}

	/* Not-vulnerable platforms (MTL, Pre-SKL) have no GPIO community definitions */
	if (!platform->global_pins) {
		printf("Platform %s is NOT vulnerable by design.\n", platform->name);
		pci_cleanup(pacc);
		printf("\n=== RESULT: platform not vulnerable by design ===\n");
		return 2;
	}

	/* 3. Find P2SB at same bus.dev, function 1, read its BAR
	 * Note: P2SB may be hidden (vendor_id reads as 0xffff),
	 * so we identify it by BDF alone. */
	struct pci_dev *p2sb = NULL;
	for (struct pci_dev *dev = pacc->devices; dev; dev = dev->next) {
		if (dev->bus == isa->bus && dev->dev == isa->dev && dev->func == 1) {
			p2sb = dev;
			break;
		}
	}
	if (p2sb)
		sbreg_base = pci_read_long(p2sb, PCI_BASE_ADDRESS_0) & ~0xfULL;

	/* If libpci couldn't find the P2SB (hidden by FSP-S), use per-platform
	 * SBREG_BAR from Intel datasheets. Matches inteltool pcr.c approach. */
	if (sbreg_base == 0) {
		uint16_t dev = isa_device;
		/* Mobile PCH families: SBREG_BAR = 0xFD000000 */
		if ((dev & 0xfff0) == 0x5180 ||   /* ADL-P */
		    (dev & 0xfff0) == 0x5190 ||   /* RPL-P */
		    (dev & 0xfff0) == 0xa080 ||   /* TGL (0xa082-0xa08f) */
		    (dev & 0xfff0) == 0xa0a0 ||   /* TGL (0xa0a0-0xa0a7) */
		    (dev & 0xfff0) == 0x0680 ||   /* CML-DT */
		    dev == 0x0660 || dev == 0x0661 || /* CML-U */
		    (dev & 0xfff0) == 0x9d80 ||   /* CNP-LP */
		    (dev & 0xfff0) == 0xa140 ||   /* SPT (0xa143-0xa14e) */
		    (dev & 0xfff0) == 0xa2c0 ||   /* KBP (0xa2c4-0xa2d2) */
		    (dev & 0xfff0) == 0xa300 ||   /* CFL-S (0xa304-0xa30e) */
		    (dev & 0xfff0) == 0x7e00) {   /* MTL (0x7e00-0x7e07) */
			sbreg_base = 0xFD000000ULL;
		/* Desktop PCH families: SBREG_BAR = 0xE0000000 */
		} else if ((dev & 0xfff0) == 0x7a80 || /* ADL-S */
			   (dev & 0xfff0) == 0x7a00 || /* RPL-S (0x7a0c-0x7a17) */
			   (dev & 0xfff0) == 0x7e20) { /* ARL-S */
			sbreg_base = 0xE0000000ULL;
		}
		if (sbreg_base == 0) {
			fprintf(stderr, "Failed to determine SBREG_BAR for device %04x\n", dev);
			pci_cleanup(pacc);
			printf("\n=== RESULT: hardware access error ===\n");
			return 127;
		}
		printf("P2SB SBREG_BAR = 0x%08" PRIx64 " (platform-specific)\n", sbreg_base);
	} else {
		printf("P2SB SBREG_BAR = 0x%08" PRIx64 " (from device BAR)\n", sbreg_base);
	}

	pci_cleanup(pacc);

	/* 4. Open /dev/mem for PCR access */
	devmemfd = open("/dev/mem", O_RDWR);
	if (devmemfd < 0) {
		perror("/dev/mem");
		printf("\n=== RESULT: hardware access error ===\n");
		return 127;
	}

	/* 5. Scan platform (detect pins via PCR) */
	int err = scan_platform(platform);
	if (err < 0) {
		printf("\n=== RESULT: hardware access error ===\n");
		return 127;
	}

	printf("\n=== RESULT: %s (unlocked: %d) ===\n",
	       err == 0 ? "all pins locked" : "unlocked pins found", err);
	return err;
}
