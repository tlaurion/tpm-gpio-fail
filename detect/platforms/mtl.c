// SPDX-License-Identifier: GPL-2.0-only
//
// Meteor Lake (MTL) -- Core Ultra Series 1+
// NOT VULNERABLE by architecture: functional GPIO lock.
// GPIO lock infrastructure compiled (SOC_INTEL_COMMON_BLOCK_GPIO_LOCK_USING_PCR),
// per-pad enforcement unconfigured. eSPI-connected TPM (Infineon SLB 9672).
// Device IDs: 0x7e00-0x7e07
// No PLTRST# pin definitions -- not applicable to this architecture.
// The C PoC detect tool reports presence but skips lock checking.
#include "defs.h"

struct platform platform_mtl = {
	.name = "MeteorLake",
	.pchs = (struct pch[]) {
		{"MTL PCH-U", 0x7e00},
		{"MTL PCH-U", 0x7e01},
		{"MTL PCH-U", 0x7e02},
		{"MTL PCH-U", 0x7e03},
		{"MTL PCH-U", 0x7e04},
		{"MTL PCH-U", 0x7e05},
		{"MTL PCH-U", 0x7e06},
		{"MTL PCH-U", 0x7e07},
		{0}
	},
	.global_pins = NULL,
	.espi_check_port = 0xc7,
	.espi_check_offset = 0x3418,
	.espi_check_bit = 1<<1,
	.lpc_pins = NULL,
	.espi_pins = NULL,
	.tier = -1,
	.summary = "MTL: functional GPIO lock, eSPI TPM, not vulnerable",
	.pad_cfg_base = 0x700,
	.pad_stride = 16
};
