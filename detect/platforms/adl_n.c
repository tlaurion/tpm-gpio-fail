// SPDX-License-Identifier: GPL-2.0-only
#include "defs.h"

struct platform platform_adl_n = {
	.name = "AlderLake N",
	.pchs = (struct pch[]) {
		// All ADL-N PCH device IDs in range 0x5480-0x549f
		{"ADL-N PCH", 0x5480},
		{"ADL-N PCH", 0x5481},
		{"ADL-N PCH", 0x5482},
		{"ADL-N PCH", 0x5483},
		{"ADL-N PCH", 0x5484},
		{"ADL-N PCH", 0x5485},
		{"ADL-N PCH", 0x5486},
		{"ADL-N PCH", 0x5487},
		{"ADL-N PCH", 0x5488},
		{"ADL-N PCH", 0x5489},
		{"ADL-N PCH", 0x548a},
		{"ADL-N PCH", 0x548b},
		{"ADL-N PCH", 0x548c},
		{"ADL-N PCH", 0x548d},
		{"ADL-N PCH", 0x548e},
		{"ADL-N PCH", 0x548f},
        {"ADL-N PCH", 0x5490},
		{"ADL-N PCH", 0x5491},
		{"ADL-N PCH", 0x5492},
		{"ADL-N PCH", 0x5493},
		{"ADL-N PCH", 0x5494},
		{"ADL-N PCH", 0x5495},
		{"ADL-N PCH", 0x5496},
		{"ADL-N PCH", 0x5497},
		{"ADL-N PCH", 0x5498},
		{"ADL-N PCH", 0x5499},
		{"ADL-N PCH", 0x549a},
		{"ADL-N PCH", 0x549b},
		{"ADL-N PCH", 0x549c},
		{"ADL-N PCH", 0x549d},
		{"ADL-N PCH", 0x549e},
		{"ADL-N PCH", 0x549f},
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
		{"GPP_A1",  "ESPI_IO0",    0x6e, 0x90, 1<<1  },
		{"GPP_A2",  "ESPI_IO1",    0x6e, 0x90, 1<<2  },
		{"GPP_A3",  "ESPI_IO2",    0x6e, 0x90, 1<<3  },
		{"GPP_A4",  "ESPI_IO3",    0x6e, 0x90, 1<<4  },
		{"GPP_A5",  "ESPI_CS#",    0x6e, 0x90, 1<<5  },
		{"GPP_A9",  "ESPI_CLK",    0x6e, 0x90, 1<<9  },
		{"GPP_A14", "ESPI_RESET#", 0x6e, 0x90, 1<<14 },
		{0}
	},
	.tier = 3,
	.summary = "ADL-N mobile: attack not confirmed on this PCH die",
	.pad_cfg_base = 0x700,
	.pad_stride = 16
};
