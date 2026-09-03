// SPDX-License-Identifier: GPL-2.0-only
//
// Pre-Skylake placeholder (Sandy Bridge, Ivy Bridge, Haswell, Broadwell)
// NOT VULNERABLE by architecture: dedicated PLTRST# hardware pin.
// The PLTRST# signal to the TPM is a dedicated pin that cannot be
// reprogrammed to GPIO mode by software.
// Pre-SKL PCH does not use sideband GPIO communities for PLTRST routing.
// No lock checking performed -- platform is identified but skipped.
// PCR_BASE defaults do not apply (no GPIO community access path).
#include "defs.h"

struct platform platform_pre_skl = {
	.name = "Pre-Skylake",
	.pchs = (struct pch[]) {
		// Pre-SKL PCH families are NOT affected by the GPIO PLTRST attack.
		// No specific device IDs are checked -- this entry serves as a
		// documented placeholder. Detection for these platforms is
		// handled by the calling script's CONFIG_BOARD-based logic.
		{0}
	},
	.global_pins = NULL,
	.espi_check_port = 0xc7,
	.espi_check_offset = 0x3418,
	.espi_check_bit = 1<<1,
	.lpc_pins = NULL,
	.espi_pins = NULL,
	.tier = -1,
	.summary = "Pre-SKL: dedicated PLTRST# pin, not vulnerable",
	.pad_cfg_base = 0,
	.pad_stride = 0
};
