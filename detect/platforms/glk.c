// SPDX-License-Identifier: GPL-2.0-only
//
// Gemini Lake (GLK) -- Pentium/Celeron N4000/J4000 series (2017 embedded SoC)
// Part of the apollolake SoC family in coreboot (src/soc/intel/apollolake/).
//
// PCH device IDs from Dasharo coreboot src/include/device/pci_ids.h:
//   PCI_DID_INTEL_GLK_LPC  = 0x31e8  (LPC variant)
//   PCI_DID_INTEL_GLK_ESPI = 0x3197  (eSPI variant)
//
// GPIO architecture (gpio_glk.c / gpio_glk.h):
//   GLK uses named communities (NW, N, AUDIO, SCC) with numbered GPIOs,
//   completely different from the GPP_A/GPP_B style used on SKL through TGL.
//   Communities: NORTHWEST (PID_GPIO_NW=0xC4), NORTH (PID_GPIO_N=0xC5),
//                AUDIO (PID_GPIO_AUDIO=0xC9), SCC (PID_GPIO_SCC=0xC8)
//   NW_OFFSET=0, N_OFFSET=80 (from gpio_glk.h).
//   PAD_CFG_BASE=0x600 (gpio_glk.h), GPIO_NUM_PAD_CFG_REGS=4, stride=16.
//
// PLTRST# -- GPIO_98 (PMU_PLTRST_N):
//   GPIO_98 = N_OFFSET + 22 = 102 (gpio_glk.h line 121)
//   Community: NORTH (PID_GPIO_N = 0xC5), first_pad = N_OFFSET = 80
//   community-relative pad index = 102 - 80 = 22
//   DW0 offset = PAD_CFG_BASE + 22 * 16 = 0x600 + 352 = 0x760
//   Confirmed on: protectli/vault_glk/gpio.h, intel/glkrvp and google/octopus
//
// NO PADCFGLOCK on GLK/APL:
//   The GLK GPIO community struct in gpio_glk.c does NOT set pad_cfg_lock_offset,
//   and SOC_INTEL_COMMON_BLOCK_SMM_LOCK_GPIO_PADS is not selected in Kconfig.
//   Therefore PADCFGLOCK registers do NOT exist on this platform -- the PLTRST#
//   pad can always be reconfigured by software. This makes GLK unconditionally
//   vulnerable (no firmware mitigation is architecturally possible via pad lock).
//
// LPC/eSPI pins -- GPIO_147-155 (NORTH community, NF1=LPC, NF2=eSPI):
//   GPIO_147 = N_OFFSET+71 = 151  LPC_SERIRQ / ESPI_IO0   (NF1/NF2)
//   GPIO_148 = N_OFFSET+72 = 152  LPC_CLKOUT0 / ESPI_IO1  (NF1/NF2)
//   GPIO_149 = N_OFFSET+73 = 153  LPC_CLKOUT1 / ESPI_IO2  (NF1/NF2)
//   GPIO_150 = N_OFFSET+74 = 154  LPC_AD0 / ESPI_IO3      (NF1/NF2)
//   GPIO_151 = N_OFFSET+75 = 155  LPC_AD1 / ESPI_CS#      (NF1/NF2)
//   GPIO_152 = N_OFFSET+76 = 156  LPC_AD2 / ESPI_CLK      (NF1/NF2)
//   GPIO_153 = N_OFFSET+77 = 157  LPC_AD3 / ESPI_RESET#   (NF1/NF2)
//   GPIO_154 = N_OFFSET+78 = 158  LPC_CLKRUN#             (LPC only, NC for eSPI)
//   GPIO_155 = N_OFFSET+79 = 159  LPC_FRAME#              (LPC only)
//   Confirmed from: protectli/vault_glk/gpio.h (LPC),
//                   google/octopus/variants/baseboard/gpio.c (eSPI)
//
// LPC vs eSPI detection: device ID is definitive (0x31e8=LPC, 0x3197=eSPI).
//   espi_check_bit=0 signals "always LPC" for the 0x31e8 entry (no register read).
//   The 0x3197 entry has lpc_pins=NULL so is_espi=1 unconditionally.
//
// SBREG_BAR: 0xD0000000 (src/soc/intel/apollolake/Kconfig: default 0xd0000000)
#include "defs.h"

// GLK with LPC bridge (device ID 0x31e8)
struct platform platform_glk = {
	.name = "GeminiLake",
	.pchs = (struct pch[]) {
		{"GLK LPC",  0x31e8},
		{"GLK eSPI", 0x3197},
		{0}
	},
	.global_pins = (struct pin[]) {
		/* lock_offset/lock_bit unused: no_padcfglock=1; dw0_offset set explicitly */
		{ "GPIO_98", "PLTRST#", 0xc5, 0, 1 },
		{0}
	},
	/* espi_check_bit=0: 0x31e8 is always LPC; 0x3197 has lpc_pins=NULL */
	.espi_check_port = 0,
	.espi_check_offset = 0,
	.espi_check_bit = 0,
	.lpc_pins = (struct pin[]) {
		{"GPIO_147", "LPC_SERIRQ",  0xc5, 0, 1<<7  },
		{"GPIO_148", "LPC_CLKOUT0", 0xc5, 0, 1<<8  },
		{"GPIO_149", "LPC_CLKOUT1", 0xc5, 0, 1<<9  },
		{"GPIO_150", "LPC_AD0",     0xc5, 0, 1<<10 },
		{"GPIO_151", "LPC_AD1",     0xc5, 0, 1<<11 },
		{"GPIO_152", "LPC_AD2",     0xc5, 0, 1<<12 },
		{"GPIO_153", "LPC_AD3",     0xc5, 0, 1<<13 },
		{"GPIO_154", "LPC_CLKRUN#", 0xc5, 0, 1<<14 },
		{"GPIO_155", "LPC_FRAME#",  0xc5, 0, 1<<15 },
		{0}
	},
	.espi_pins = (struct pin[]) {
		{"GPIO_147", "ESPI_IO0",    0xc5, 0, 1<<7  },
		{"GPIO_148", "ESPI_IO1",    0xc5, 0, 1<<8  },
		{"GPIO_149", "ESPI_IO2",    0xc5, 0, 1<<9  },
		{"GPIO_150", "ESPI_IO3",    0xc5, 0, 1<<10 },
		{"GPIO_151", "ESPI_CS#",    0xc5, 0, 1<<11 },
		{"GPIO_152", "ESPI_CLK",    0xc5, 0, 1<<12 },
		{"GPIO_153", "ESPI_RESET#", 0xc5, 0, 1<<13 },
		{0}
	},
	.tier = 2,
	.summary = "GLK: no PADCFGLOCK register exists -- PLTRST# pad is always writable, no firmware mitigation possible",
	.pad_cfg_base = 0x600,
	.pad_stride = 16,
	.dw0_offset = 0x760,  /* GPIO_98: community-relative pad 22, 0x600 + 22*16 */
	.no_padcfglock = 1
};
