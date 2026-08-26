// SPDX-License-Identifier: GPL-2.0-only
//
// Comet Lake U/Y (CML-U/Y) -- 10th gen mobile (400-series PCH, LP variant)
// GPP_B13 (COMM_0, local idx 13) port=0x6e PAD_CFG_BASE=0x600
// PADCFGLOCK at offset 0x88 (Intel doc 834810)
// Device IDs sourced from Dasharo coreboot src/include/device/pci_ids.h
// (PCI_DID_INTEL_CMP_*_U/Y_LPC). Note: the original 0x0660/0x0661 IDs
// are not present in coreboot's pci_ids.h and likely did not correspond
// to any real CML-U/Y PCH SKU.
// Supports both LPC and eSPI.
// PCR_BASE=0xFD000000
#include "defs.h"

struct platform platform_cml_u = {
	.name = "CometLake U/Y",
	.pchs = (struct pch[]) {
		{"CML-U Super",   0x0281},
		{"CML-Y Premium", 0x0283},
		{"CML-U Premium", 0x0284},
		{"CML-U Base",    0x0285},
		{"CML-Y Super",   0x0286},
		{0}
	},
	.global_pins = (struct pin[]) {
		{ "GPP_B13", "PLTRST#",    0x6e, 0x88, 1<<13 },
		{0}
	},
	.espi_check_port = 0xc7,
	.espi_check_offset = 0x3418,
	.espi_check_bit = 1<<1,
	.lpc_pins = (struct pin[]) {
		{"GPP_A1",  "LAD0",        0x6e, 0x88, 1<<1  },
		{"GPP_A2",  "LAD1",        0x6e, 0x88, 1<<2  },
		{"GPP_A3",  "LAD2",        0x6e, 0x88, 1<<3  },
		{"GPP_A4",  "LAD3",        0x6e, 0x88, 1<<4  },
		{"GPP_A5",  "LFRAME#",     0x6e, 0x88, 1<<5  },
		{"GPP_A6",  "SERIRQ",      0x6e, 0x88, 1<<6  },
		{"GPP_A8",  "CLKRUN",      0x6e, 0x88, 1<<8  },
		{"GPP_A9",  "CLKOUT_LPC0", 0x6e, 0x88, 1<<9  },
		{0}
	},
	.espi_pins = (struct pin[]) {
		{"GPP_A1",  "ESPI_IO0",    0x6e, 0x88, 1<<1  },
		{"GPP_A2",  "ESPI_IO1",    0x6e, 0x88, 1<<2  },
		{"GPP_A3",  "ESPI_IO2",    0x6e, 0x88, 1<<3  },
		{"GPP_A4",  "ESPI_IO3",    0x6e, 0x88, 1<<4  },
		{"GPP_A5",  "ESPI_CS#",    0x6e, 0x88, 1<<5  },
		{"GPP_A9",  "ESPI_CLK",    0x6e, 0x88, 1<<9  },
		{"GPP_A14", "ESPI_RESET#", 0x6e, 0x88, 1<<14 },
		{0}
	},
	.tier = 2,
	.summary = "CML-U: untested mobile, CNP-LP-like GPIO layout",
	.pad_cfg_base = 0x600,
	.pad_stride = 16,
	.dw0_offset = 0x860  /* GPP_B13: community-relative pad 38, 0x600 + 38*16 */
};
