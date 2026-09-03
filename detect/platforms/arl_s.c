// SPDX-License-Identifier: GPL-2.0-only
//
// Arrow Lake S (ARL-S) -- 15th gen desktop
// GPP_B13 (COMM_1, local idx 13) port=0x6d PAD_CFG_BASE=0x700
// PADCFGLOCK at offset 0x120 (Intel doc 834810)
// Device IDs: 0x7e20-0x7e2f
// eSPI only (modern desktop PCH)
// PCR_BASE=0xE0000000
#include "defs.h"

struct platform platform_arl_s = {
	.name = "ArrowLake S",
	.pchs = (struct pch[]) {
		{"ARL-S PCH", 0x7e20},
		{"ARL-S PCH", 0x7e21},
		{"ARL-S PCH", 0x7e22},
		{"ARL-S PCH", 0x7e23},
		{"ARL-S PCH", 0x7e24},
		{"ARL-S PCH", 0x7e25},
		{"ARL-S PCH", 0x7e26},
		{"ARL-S PCH", 0x7e27},
		{"ARL-S PCH", 0x7e28},
		{"ARL-S PCH", 0x7e29},
		{"ARL-S PCH", 0x7e2a},
		{"ARL-S PCH", 0x7e2b},
		{"ARL-S PCH", 0x7e2c},
		{"ARL-S PCH", 0x7e2d},
		{"ARL-S PCH", 0x7e2e},
		{"ARL-S PCH", 0x7e2f},
		{0}
	},
	.global_pins = (struct pin[]) {
		{ "GPP_B13", "PLTRST#",    0x6d, 0x120, 1<<13 },
		{0}
	},
	.espi_check_port = 0xc7,
	.espi_check_offset = 0x3418,
	.espi_check_bit = 1<<1,
	.lpc_pins = NULL,
	.espi_pins = (struct pin[]) {
		{"GPP_A1",  "ESPI_IO0",    0x6d, 0x120, 1<<1  },
		{"GPP_A2",  "ESPI_IO1",    0x6d, 0x120, 1<<2  },
		{"GPP_A3",  "ESPI_IO2",    0x6d, 0x120, 1<<3  },
		{"GPP_A4",  "ESPI_IO3",    0x6d, 0x120, 1<<4  },
		{"GPP_A5",  "ESPI_CS#",    0x6d, 0x120, 1<<5  },
		{"GPP_A9",  "ESPI_CLK",    0x6d, 0x120, 1<<9  },
		{"GPP_A14", "ESPI_RESET#", 0x6d, 0x120, 1<<14 },
		{0}
	},
	.tier = 3,
	.summary = "ARL-S desktop: attack not confirmed on this PCH die",
	.pad_cfg_base = 0x700,
	.pad_stride = 16
};
