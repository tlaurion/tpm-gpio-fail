// SPDX-License-Identifier: GPL-2.0-only
//
// Raptor Lake-P mobile (13th/14th gen mobile)
// GPP_B13 (COMM_1, local idx 13) port=0x6e lock_offset=0x80
// Device IDs: 0x5190-0x519f
// eSPI only (modern mobile PCH)
#include "defs.h"

struct platform platform_rpl_p = {
	.name = "RaptorLake P",
	.pchs = (struct pch[]) {
		{"RPL-P PCH", 0x5190},
		{"RPL-P PCH", 0x5191},
		{"RPL-P PCH", 0x5192},
		{"RPL-P PCH", 0x5193},
		{"RPL-P PCH", 0x5194},
		{"RPL-P PCH", 0x5195},
		{"RPL-P PCH", 0x5196},
		{"RPL-P PCH", 0x5197},
		{"RPL-P PCH", 0x5198},
		{"RPL-P PCH", 0x5199},
		{"RPL-P PCH", 0x519a},
		{"RPL-P PCH", 0x519b},
		{"RPL-P PCH", 0x519c},
		{"RPL-P PCH", 0x519d},
		{"RPL-P PCH", 0x519e},
		{"RPL-P PCH", 0x519f},
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
	.tier = 3,
	.summary = "RPL-P mobile: same ADL-P arch, attack not confirmed",
	.pad_cfg_base = 0x700,
	.pad_stride = 16
};
