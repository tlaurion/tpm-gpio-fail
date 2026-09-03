// SPDX-License-Identifier: GPL-2.0-only
//
// Raptor Lake-S desktop (13th/14th gen)
// GPP_B13 (COMM_1, local idx 13) port=0x6d PAD_CFG_BASE=0x700
// PADCFGLOCK at offset 0x110
// Device IDs: 0x7a0c-0x7a17
// eSPI only (modern desktop PCH)
// PCR_BASE=0xE0000000
#include "defs.h"

struct platform platform_rpl_s = {
	.name = "RaptorLake S",
	.pchs = (struct pch[]) {
		{"RPL-S PCH", 0x7a0c},
		{"RPL-S PCH", 0x7a0d},
		{"RPL-S PCH", 0x7a0e},
		{"RPL-S PCH", 0x7a0f},
		{"RPL-S PCH", 0x7a10},
		{"RPL-S PCH", 0x7a11},
		{"RPL-S PCH", 0x7a12},
		{"RPL-S PCH", 0x7a13},
		{"RPL-S PCH", 0x7a14},
		{"RPL-S PCH", 0x7a15},
		{"RPL-S PCH", 0x7a16},
		{"RPL-S PCH", 0x7a17},
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
	.summary = "RPL-S desktop: attack not confirmed on this PCH die",
	.pad_cfg_base = 0x700,
	.pad_stride = 16
};
