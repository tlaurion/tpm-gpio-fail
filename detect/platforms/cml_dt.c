// SPDX-License-Identifier: GPL-2.0-only
//
// Comet Lake Desktop/H (CML-DT/H) -- B460, H410, H470, Z490, W480 (10th gen desktop)
// The 400-series CML desktop PCH shares its GPIO IP with the 300-series CNL/CFL
// (both are in src/soc/intel/cannonlake in coreboot). Values therefore mirror
// cfl.c exactly -- NOT skl_kbl.c, despite CML-DT being a desktop platform.
// Key differences from SKL/KBL (SPT PCH):
//   - port:         0x6e (PID_GPIOCOM0, cannonlake/include/soc/pcr_ids.h)
//                   NOT 0xaf (that is the SPT GPIO community port)
//   - pad_cfg_base: 0x600 (cannonlake/include/soc/gpio_defs_cnp_h.h)
//                   NOT 0x400
//   - pad_stride:   16 (4 DW per pad, GPIO_NUM_PAD_CFG_REGS=4)
//                   NOT 8 (2 DW per pad as in SPT)
//   - lock_offset:  0x88 for GPP_B (COMM_0 group 1, base 0x80 + 1*8)
//                   NOT 0xb0
//   - lock_bit:     1<<13 for GPP_B13
//                   NOT 1<<5
// Community layout (gpio_cnp_h.c, COMM_0/0x6e):
//   group 0: GPP_A (pads 0..24, ESPI_CLK_LOOPBK at 24)
//   group 1: GPP_B (pads 25..50, GSPI1_CLK_LOOPBK at 50)
// GPP_B13 community-relative pad index = 38 -> DW0 offset = 0x600 + 38*16 = 0x860
// Device IDs sourced from Dasharo coreboot src/include/device/pci_ids.h.
// Supports both LPC and eSPI.
// PCR_BASE=0xE0000000 (desktop SBREG_BAR)
#include "defs.h"

struct platform platform_cml_dt = {
	.name = "CometLake Desktop/H",
	.pchs = (struct pch[]) {
		{"CML-H H470",   0x0684},
		{"CML-H Z490",   0x0685},
		{"CML-DT PCH",   0x0686},
		{"CML-H Q470",   0x0687},
		{"CML-DT PCH",   0x0688},
		{"CML-DT PCH",   0x0689},
		{"CML-DT PCH",   0x068a},
		{"CML-DT PCH",   0x068b},
		{"CML-H QM480",  0x068c},
		{"CML-H HM470",  0x068d},
		{"CML-H WM490",  0x068e},
		{"CML-DT PCH",   0x068f},
		{"CML-H W480",   0x0697},
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
	.summary = "CML-DT/H: 400-series CNL/CFL-compatible GPIO IP, corrected from SPT values",
	.pad_cfg_base = 0x600,
	.pad_stride = 16,
	.dw0_offset = 0x860  /* GPP_B13: community-relative pad 38, 0x600 + 38*16 */
};
