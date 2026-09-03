// SPDX-License-Identifier: GPL-2.0-only
//
// Tiger Lake (TGL) -- 11th gen mobile/desktop
// GPP_B13 (COMM_0, local idx 13) port=0x6e PAD_CFG_BASE=0x700
// PADCFGLOCK at offset 0x80 (Intel doc 834810)
// Device IDs: 0xa082-0xa08f, 0xa0a0-0xa0a7
// Supports both LPC and eSPI.
// PCR_BASE=0xFD000000
#include "defs.h"

struct platform platform_tgl = {
	.name = "TigerLake",
	.pchs = (struct pch[]) {
		{"TGL PCH", 0xa082},
		{"TGL PCH", 0xa083},
		{"TGL PCH", 0xa084},
		{"TGL PCH", 0xa085},
		{"TGL PCH", 0xa086},
		{"TGL PCH", 0xa087},
		{"TGL PCH", 0xa088},
		{"TGL PCH", 0xa089},
		{"TGL PCH", 0xa08a},
		{"TGL PCH", 0xa08b},
		{"TGL PCH", 0xa08c},
		{"TGL PCH", 0xa08d},
		{"TGL PCH", 0xa08e},
		{"TGL PCH", 0xa08f},
		{"TGL PCH", 0xa0a0},
		{"TGL PCH", 0xa0a1},
		{"TGL PCH", 0xa0a2},
		{"TGL PCH", 0xa0a3},
		{"TGL PCH", 0xa0a4},
		{"TGL PCH", 0xa0a5},
		{"TGL PCH", 0xa0a6},
		{"TGL PCH", 0xa0a7},
		{0}
	},
	.global_pins = (struct pin[]) {
		{ "GPP_B13", "PLTRST#",    0x6e, 0x80, 1<<13 },
		{0}
	},
	.espi_check_port = 0xc7,
	.espi_check_offset = 0x3418,
	.espi_check_bit = 1<<1,
	.lpc_pins = (struct pin[]) {
		{"GPP_A1",  "LAD0",        0x6e, 0x80, 1<<1  },
		{"GPP_A2",  "LAD1",        0x6e, 0x80, 1<<2  },
		{"GPP_A3",  "LAD2",        0x6e, 0x80, 1<<3  },
		{"GPP_A4",  "LAD3",        0x6e, 0x80, 1<<4  },
		{"GPP_A5",  "LFRAME#",     0x6e, 0x80, 1<<5  },
		{"GPP_A6",  "SERIRQ",      0x6e, 0x80, 1<<6  },
		{"GPP_A8",  "CLKRUN",      0x6e, 0x80, 1<<8  },
		{"GPP_A9",  "CLKOUT_LPC0", 0x6e, 0x80, 1<<9  },
		{0}
	},
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
	.tier = 3,
	.summary = "TGL: attack not confirmed on this PCH die",
	.pad_cfg_base = 0x700,
	.pad_stride = 16
};
