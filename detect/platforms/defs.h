// SPDX-License-Identifier: GPL-2.0-only
#pragma once
#include <stddef.h>
#include <stdint.h>

struct pin {
	const char *gpio;
	const char *nf;
	uint8_t port;
	uint32_t lock_offset;
	uint32_t lock_bit;
};

struct pch {
	const char *name;
	uint16_t pid;
};

struct platform {
	const char *name;
	struct pch *pchs;
	struct pin *global_pins;
	uint8_t espi_check_port;
	uint32_t espi_check_offset;
	uint32_t espi_check_bit;
	struct pin *lpc_pins;
	struct pin *espi_pins;
	int tier;              /* -1=not vuln, 1=confirmed, 2=unconfirmed, 3=uncertain */
	const char *summary;   /* one-line vulnerability description */
	uint32_t pad_cfg_base; /* PAD_CFG_BASE offset within community */
	uint8_t  pad_stride;   /* bytes per pad (NUM_PAD_CFG_REGS * 4) */
};
