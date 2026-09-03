// SPDX-License-Identifier: GPL-2.0-only
//
// Alder Lake-S desktop (12th gen)
// GPP_B13 (COMM_1, local idx 13) port=0x6d PAD_CFG_BASE=0x700
// PADCFGLOCK at offset 0x110 (same as RPL-S)
// Device IDs: 0x7a80-0x7a8c
// eSPI only (modern desktop PCH)
// PCR_BASE=0xE0000000
#include "defs.h"

struct platform platform_adl_s = {
	.name = "AlderLake S",
	.pchs = (struct pch[]) {
		{"ADL-S PCH", 0x7a80},
		{"ADL-S PCH", 0x7a81},
		{"ADL-S PCH", 0x7a82},
		{"ADL-S PCH", 0x7a83},
		{"ADL-S PCH", 0x7a84},
		{"ADL-S PCH", 0x7a85},
		{"ADL-S PCH", 0x7a86},
		{"ADL-S PCH", 0x7a87},
		{"ADL-S PCH", 0x7a88},
		{"ADL-S PCH", 0x7a89},
		{"ADL-S PCH", 0x7a8a},
		{"ADL-S PCH", 0x7a8b},
		{"ADL-S PCH", 0x7a8c},
		{0}
	},
	.global_pins = (struct pin[]) {
		{ "GPP_B13", "PLTRST#",    0x6d, 0x110, 1<<13 },
		{0}
	},
	.espi_check_port = 0xc7,
	.espi_check_offset = 0x3418,
	.espi_check_bit = 1<<1,
	.lpc_pins = NULL,
	.espi_pins = (struct pin[]) {
		{"GPP_A1",  "ESPI_IO0",    0x6d, 0x110, 1<<1  },
		{"GPP_A2",  "ESPI_IO1",    0x6d, 0x110, 1<<2  },
		{"GPP_A3",  "ESPI_IO2",    0x6d, 0x110, 1<<3  },
		{"GPP_A4",  "ESPI_IO3",    0x6d, 0x110, 1<<4  },
		{"GPP_A5",  "ESPI_CS#",    0x6d, 0x110, 1<<5  },
		{"GPP_A9",  "ESPI_CLK",    0x6d, 0x110, 1<<9  },
		{"GPP_A14", "ESPI_RESET#", 0x6d, 0x110, 1<<14 },
		{0}
	},
	.tier = 3,
	.summary = "ADL-S desktop: attack not confirmed on this PCH die",
	.pad_cfg_base = 0x700,
	.pad_stride = 16
};
