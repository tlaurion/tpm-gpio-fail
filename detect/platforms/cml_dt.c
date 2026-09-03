// SPDX-License-Identifier: GPL-2.0-only
//
// Comet Lake Desktop (CML-DT) -- B460, H410, H470, Z490 (10th gen desktop)
// GPP_B13 (COMM_0, local idx 13) port=0xaf PAD_CFG_BASE=0x400
// PADCFGLOCK at offset 0xA8 (same as SPT).
// Same port as SPT/KBP but different PCH generation.
// Device IDs: 0x0684-0x068f
// Supports both LPC and eSPI.
// PCR_BASE=0xFD000000
#include "defs.h"

struct platform platform_cml_dt = {
	.name = "CometLake Desktop",
	.pchs = (struct pch[]) {
		{"CML-DT PCH", 0x0684},
		{"CML-DT PCH", 0x0685},
		{"CML-DT PCH", 0x0686},
		{"CML-DT PCH", 0x0687},
		{"CML-DT PCH", 0x0688},
		{"CML-DT PCH", 0x0689},
		{"CML-DT PCH", 0x068a},
		{"CML-DT PCH", 0x068b},
		{"CML-DT PCH", 0x068c},
		{"CML-DT PCH", 0x068d},
		{"CML-DT PCH", 0x068e},
		{"CML-DT PCH", 0x068f},
		{0}
	},
	.global_pins = (struct pin[]) {
		{ "GPP_B13", "PLTRST#",    0xaf, 0xb0, 1<<5 },
		{0}
	},
	.espi_check_port = 0xc7,
	.espi_check_offset = 0x3418,
	.espi_check_bit = 1<<1,
	.lpc_pins = (struct pin[]) {
		{"GPP_A1",  "LAD0",        0xaf, 0xa0, 1<<1  },
		{"GPP_A2",  "LAD1",        0xaf, 0xa0, 1<<2  },
		{"GPP_A3",  "LAD2",        0xaf, 0xa0, 1<<3  },
		{"GPP_A4",  "LAD3",        0xaf, 0xa0, 1<<4  },
		{"GPP_A5",  "LFRAME#",     0xaf, 0xa0, 1<<5  },
		{"GPP_A6",  "SERIRQ",      0xaf, 0xa0, 1<<6  },
		{"GPP_A8",  "CLKRUN",      0xaf, 0xa0, 1<<8  },
		{"GPP_A9",  "CLKOUT_LPC0", 0xaf, 0xa0, 1<<9  },
		{0}
	},
	.espi_pins = (struct pin[]) {
		{"GPP_A1",  "ESPI_IO0",    0xaf, 0xa0, 1<<1  },
		{"GPP_A2",  "ESPI_IO1",    0xaf, 0xa0, 1<<2  },
		{"GPP_A3",  "ESPI_IO2",    0xaf, 0xa0, 1<<3  },
		{"GPP_A4",  "ESPI_IO3",    0xaf, 0xa0, 1<<4  },
		{"GPP_A5",  "ESPI_CS#",    0xaf, 0xa0, 1<<5  },
		{"GPP_A9",  "ESPI_CLK",    0xaf, 0xa0, 1<<9  },
		{"GPP_A14", "ESPI_RESET#", 0xaf, 0xa0, 1<<14 },
		{0}
	},
	.tier = 2,
	.summary = "CML-DT: SPT-like GPIO arch, no community test data",
	.pad_cfg_base = 0x400,
	.pad_stride = 8
};
