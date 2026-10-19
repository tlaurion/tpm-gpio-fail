// SPDX-License-Identifier: GPL-2.0-only
//
// Jasper Lake (JSL) -- Celeron/Pentium N-series/J-series (2020 low-power SoC)
// Data derived from Dasharo coreboot (src/soc/intel/jasperlake):
//   - PCH device ID: include/device/pci_ids.h, single SKU
//     PCI_DID_INTEL_JSP_SUPER_ESPI = 0x4d87
//   - PCR ports: include/soc/pcr_ids.h (PID_GPIOCOM0=0x6e, PID_ESPI=0xc7)
//   - GPIO layout: gpio.c / gpio_soc_defs.h -- COMM_0 (0x6e) groups[]:
//       { GPP_F(0), SPI0(1), GPP_B(2), GPP_A(3), GPP_S(4), GPP_R(5) }
//     PADCFGLOCK base = 0x80 (PAD_CFG_LOCK in gpio_defs.h, source-confirmed).
//     Per-group lock offsets: GPP_B -> 0x80+2*8=0x90, GPP_A -> 0x80+3*8=0x98.
//     Lock bits are the pin's index within its own group.
//   - PAD_CFG_BASE = 0x600, pad_stride = 16. COMM_0 first_pad = GPP_F0 = 0.
//     GPP_B13 community-relative pad index = 42 -> DW0 offset = 0x600+42*16 = 0x8a0.
//   - eSPI pin mapping confirmed from mainboard/protectli/vault_jsl/gpio.h
//     and mainboard/intel/jasperlake_rvp/variants/jslrvp/gpio.c.
//     IMPORTANT: JSL uses a different GPP_A eSPI mapping than CNL/CML/TGL:
//       JSL:     GPP_A0=IO0, A1=IO1, A2=IO2, A3=IO3, A4=CS#, A5=CLK, A6=RESET#
//       CNL/CML: GPP_A1=IO0, A2=IO1, A3=IO2, A4=IO3, A5=CS#, A9=CLK, A14=RESET#
//   - GPP_B13 = PMC_PLTRST_N confirmed on both reference boards.
// Device ID: 0x4d87 (single SKU). eSPI only (no LPC on JSL).
// PCR_BASE=0xFD000000 (mobile/embedded SBREG_BAR bucket)
#include "defs.h"

struct platform platform_jsl = {
	.name = "JasperLake",
	.pchs = (struct pch[]) {
		{"JSL PCH", 0x4d87},
		{0}
	},
	.global_pins = (struct pin[]) {
		{ "GPP_B13", "PLTRST#",    0x6e, 0x90, 1<<13 },
		{0}
	},
	.espi_check_port = 0xc7,
	.espi_check_offset = 0x3418,
	.espi_check_bit = 1<<1,
	.lpc_pins = NULL,
	.espi_pins = (struct pin[]) {
		{"GPP_A0",  "ESPI_IO0",    0x6e, 0x98, 1<<0  },
		{"GPP_A1",  "ESPI_IO1",    0x6e, 0x98, 1<<1  },
		{"GPP_A2",  "ESPI_IO2",    0x6e, 0x98, 1<<2  },
		{"GPP_A3",  "ESPI_IO3",    0x6e, 0x98, 1<<3  },
		{"GPP_A4",  "ESPI_CS#",    0x6e, 0x98, 1<<4  },
		{"GPP_A5",  "ESPI_CLK",    0x6e, 0x98, 1<<5  },
		{"GPP_A6",  "ESPI_RESET#", 0x6e, 0x98, 1<<6  },
		{0}
	},
	.tier = 2,
	.summary = "JSL: PADCFGLOCK confirmed in coreboot source, eSPI pins confirmed from reference board gpio.h",
	.pad_cfg_base = 0x600,
	.pad_stride = 16,
	.dw0_offset = 0x8a0
};

