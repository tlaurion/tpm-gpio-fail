// SPDX-License-Identifier: GPL-2.0-only
//
// Cannon Point LP (CNP-LP) -- Kaby Lake-R / Whiskey Lake (300-series)
// GPP_B13 (COMM_0, local idx 38) port=0x6e PAD_CFG_BASE=0x600
// NOTE: No PADCFGLOCK register exists on CNP-LP proper.
// _get_lock_base returns 0x80 but the register read returns 0x0 / is absent.
// Device IDs: 0x9d84-0x9d8f
// UNTESTED: kukri's PoC does not support this PCH family.
// Supports both LPC and eSPI.
// PCR_BASE=0xFD000000
#include "defs.h"

struct platform platform_cnp_lp = {
	.name = "Cannon Point LP",
	.pchs = (struct pch[]) {
		{"CNP-LP PCH", 0x9d84},
		{"CNP-LP PCH", 0x9d85},
		{"CNP-LP PCH", 0x9d86},
		{"CNP-LP PCH", 0x9d87},
		{"CNP-LP PCH", 0x9d88},
		{"CNP-LP PCH", 0x9d89},
		{"CNP-LP PCH", 0x9d8a},
		{"CNP-LP PCH", 0x9d8b},
		{"CNP-LP PCH", 0x9d8c},
		{"CNP-LP PCH", 0x9d8d},
		{"CNP-LP PCH", 0x9d8e},
		{"CNP-LP PCH", 0x9d8f},
		{0}
	},
	.global_pins = (struct pin[]) {
		{ "GPP_B13", "PLTRST#",    0x6e, 0x88, 1<<6 },
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
	.tier = 2,
	.summary = "CNP-LP: PADCFGLOCK absent per Intel 834810, untested",
	.pad_cfg_base = 0x600,
	.pad_stride = 16
};
