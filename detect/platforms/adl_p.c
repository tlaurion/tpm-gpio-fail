// SPDX-License-Identifier: GPL-2.0-only
#include "defs.h"

struct platform platform_adl_p = {
	.name = "AlderLake P",
	.pchs = (struct pch[]) {
		// All ADL-P PCH device IDs in range 0x5180-0x518f
		{"ADL-P PCH", 0x5180},
		{"ADL-P PCH", 0x5181},
		{"ADL-P PCH", 0x5182},
		{"ADL-P PCH", 0x5183},
		{"ADL-P PCH", 0x5184},
		{"ADL-P PCH", 0x5185},
		{"ADL-P PCH", 0x5186},
		{"ADL-P PCH", 0x5187},
		{"ADL-P PCH", 0x5188},
		{"ADL-P PCH", 0x5189},
		{"ADL-P PCH", 0x518a},
		{"ADL-P PCH", 0x518b},
		{"ADL-P PCH", 0x518c},
		{"ADL-P PCH", 0x518d},
		{"ADL-P PCH", 0x518e},
		{"ADL-P PCH", 0x518f},
		{0}
	},
	.global_pins = (struct pin[]) {
		{ "GPP_B13", "PLTRST#",    0x6e, 0x80, 1<<13 },
		{0}
	},
	.espi_check_port = 0xc7,
	.espi_check_offset = 0x3418,
	.espi_check_bit = 1<<1,
	.lpc_pins = NULL,
	.espi_pins = (struct pin[]) {
		{"GPP_A1",  "ESPI_IO0",    0x6e, 0x80, 1<<1  },
		{"GPP_A2",  "ESPI_IO1",    0x6e, 0x80, 1<<2  },
		{"GPP_A3",  "ESPI_IO2",    0x6e, 0x80, 1<<3  },
		{"GPP_A4",  "ESPI_IO3",    0x6e, 0x80, 1<<4  },
		{"GPP_A5",  "ESPI_CS#",    0x6e, 0x80, 1<<5  },
		{"GPP_A9",  "ESPI_CLK",    0x6e, 0x80, 1<<9  },
		{"GPP_A14", "ESPI_RESET#", 0x6e, 0x80, 1<<14 },
		{0}
	},
	.tier = 1,
	.summary = "ADL-P mobile: NF1 mode confirmed, PLTRST# verified (TIER 1)",
	.pad_cfg_base = 0x700,
	.pad_stride = 16
};
