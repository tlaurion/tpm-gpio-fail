// SPDX-License-Identifier: GPL-2.0-only
//
// Elkhart Lake (EHL) -- Atom x6000E / Pentium/Celeron J-series embedded SoC
// Data derived from Dasharo coreboot (src/soc/intel/elkhartlake):
//   - PCH device IDs: include/device/pci_ids.h (PCI_DID_INTEL_MCC_*)
//   - PCR ports: include/soc/pcr_ids.h (PID_GPIOCOM0=0x6e, PID_GPIOCOM3=0x6b,
//     PID_ESPI=0x72 -- NOTE: unlike SKL..ADL, ESPI port is NOT 0xc7 here)
//   - GPIO layout: gpio.c / gpio_soc_defs.h -- IMPORTANT: unlike every other
//     platform in this tree, GPP_A and GPP_B live in DIFFERENT PCR
//     communities on EHL (GPP_B is COMM_0/0x6e, GPP_A is COMM_3/0x6b), so
//     PLTRST# and the LPC/eSPI pins are read from different PCR ports.
//   - PAD_CFG_BASE = 0x700 (gpio_defs.h), pad_stride = 4 DW * 4 = 16.
//
// UNCONFIRMED: coreboot's own gpio.c does not set .pad_cfg_lock_offset for
// any EHL community (i.e. coreboot never locks these pads itself), so the
// PADCFGLOCK base offset is not sourced from coreboot. The 0x80 base used
// below is inferred by analogy with Jasper Lake (same generation PCH IP,
// confirmed PAD_CFG_LOCK=0x80) and with SKL..TGL, using the standard
// "base + group_index*8" stepping observed in coreboot's gpio_lock_pads()
// for GPP_B (group 0 of COMM_0 -> +0x00) and GPP_A (group 2 of COMM_3 ->
// +0x10). It has NOT been confirmed against an Intel EHL PCH EDS or by
// register read on real hardware. The eSPI-enable check offset (0x3418)
// is likewise carried over unverified from other platforms; only the port
// (0x72) is confirmed to be correct for EHL.
// Device IDs: 0x4b00-0x4b07
// eSPI only (EHL has no LPC bridge in mainline coreboot mainboards).
// PCR_BASE=0xFD000000 (mobile/embedded SBREG_BAR bucket)
#include "defs.h"

struct platform platform_ehl = {
	.name = "ElkhartLake",
	.pchs = (struct pch[]) {
		{"EHL PCH-0",       0x4b00},
		{"EHL PCH Super",   0x4b01},
		{"EHL PCH Premium", 0x4b02},
		{"EHL PCH Base",    0x4b03},
		{"EHL PCH-1",       0x4b04},
		{"EHL PCH-2",       0x4b05},
		{"EHL PCH-3",       0x4b06},
		{"EHL PCH-4",       0x4b07},
		{0}
	},
	.global_pins = (struct pin[]) {
		{ "GPP_B13", "PLTRST#",    0x6e, 0x80, 1<<13 },
		{0}
	},
	.espi_check_port = 0x72,
	.espi_check_offset = 0x3418,
	.espi_check_bit = 1<<1,
	.lpc_pins = NULL,
	.espi_pins = (struct pin[]) {
		{"GPP_A1",  "ESPI_IO0",    0x6b, 0x90, 1<<1  },
		{"GPP_A2",  "ESPI_IO1",    0x6b, 0x90, 1<<2  },
		{"GPP_A3",  "ESPI_IO2",    0x6b, 0x90, 1<<3  },
		{"GPP_A4",  "ESPI_IO3",    0x6b, 0x90, 1<<4  },
		{"GPP_A5",  "ESPI_CS#",    0x6b, 0x90, 1<<5  },
		{"GPP_A9",  "ESPI_CLK",    0x6b, 0x90, 1<<9  },
		{"GPP_A14", "ESPI_RESET#", 0x6b, 0x90, 1<<14 },
		{0}
	},
	.tier = 3,
	.summary = "EHL: PLTRST#/eSPI pins split across two PCR communities, PADCFGLOCK offset inferred, unconfirmed",
	.pad_cfg_base = 0x700,
	.pad_stride = 16
};
