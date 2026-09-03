/* inteltool - dump all registers on an Intel CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>
#include "pcr.h"

const uint8_t *sbbar = NULL;

uint32_t read_pcr32(const uint8_t port, const uint16_t offset)
{
	assert(sbbar);
	return *(const uint32_t *)(sbbar + (port << 16) + offset);
}

void write_pcr32(uint8_t port, uint16_t offset, uint32_t value)
{
	*(uint32_t *)(sbbar + (port << 16) + offset) = value;
}

static void print_pcr_port(const uint8_t port)
{
	size_t i = 0;
	uint32_t last_reg = 0;
	bool last_printed = true;

	printf("PCR port offset: 0x%06zx\n\n", (size_t)port << 16);

	for (i = 0; i < PCR_PORT_SIZE; i += 4) {
		const uint32_t reg = read_pcr32(port, i);
		const bool rep = i && last_reg == reg;
		if (!rep) {
			if (!last_printed)
				printf("*\n");
			printf("0x%04zx: 0x%08"PRIx32"\n", i, reg);
		}

		last_reg = reg;
		last_printed = !rep;
	}
	if (!last_printed)
		printf("*\n");
}

void print_pcr_ports(struct pci_dev *const sb,
		     const uint8_t *const ports, const size_t count)
{
	size_t i;

	pcr_init(sb);

	for (i = 0; i < count; ++i) {
		printf("\n========== PCR 0x%02x ==========\n\n", ports[i]);
		print_pcr_port(ports[i]);
	}
}

void pcr_init(struct pci_dev *const sb)
{
	bool error_exit = false;
	bool p2sb_revealed = false;
	struct pci_dev *p2sb;
	bool use_p2sb = true;
	pciaddr_t sbbar_phys;

	if (sbbar)
		return;

	switch (sb->device_id) {
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_PRE:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_PRE:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_BASE_SKL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_PREM_SKL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_PREM_SKL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_BASE_KBL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_PREM_KBL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_PREM_KBL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_IHDCP_BASE:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_IHDCP_PREM:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_IHDCP_PREM:
	case PCI_DEVICE_ID_INTEL_H110:
	case PCI_DEVICE_ID_INTEL_H170:
	case PCI_DEVICE_ID_INTEL_Z170:
	case PCI_DEVICE_ID_INTEL_Q170:
	case PCI_DEVICE_ID_INTEL_Q150:
	case PCI_DEVICE_ID_INTEL_B150:
	case PCI_DEVICE_ID_INTEL_C236:
	case PCI_DEVICE_ID_INTEL_C232:
	case PCI_DEVICE_ID_INTEL_QM170:
	case PCI_DEVICE_ID_INTEL_HM170:
	case PCI_DEVICE_ID_INTEL_CM236:
	case PCI_DEVICE_ID_INTEL_HM175:
	case PCI_DEVICE_ID_INTEL_QM175:
	case PCI_DEVICE_ID_INTEL_CM238:
	case PCI_DEVICE_ID_INTEL_H270:
	case PCI_DEVICE_ID_INTEL_Z270:
	case PCI_DEVICE_ID_INTEL_Q270:
	case PCI_DEVICE_ID_INTEL_Q250:
	case PCI_DEVICE_ID_INTEL_B250:
	case PCI_DEVICE_ID_INTEL_Z370:
	case PCI_DEVICE_ID_INTEL_H310C:
	case PCI_DEVICE_ID_INTEL_X299:
	case PCI_DEVICE_ID_INTEL_C621:
	case PCI_DEVICE_ID_INTEL_C621A:
	case PCI_DEVICE_ID_INTEL_C622:
	case PCI_DEVICE_ID_INTEL_C624:
	case PCI_DEVICE_ID_INTEL_C625:
	case PCI_DEVICE_ID_INTEL_C626:
	case PCI_DEVICE_ID_INTEL_C627:
	case PCI_DEVICE_ID_INTEL_C628:
	case PCI_DEVICE_ID_INTEL_C629:
	case PCI_DEVICE_ID_INTEL_C624_SUPER:
	case PCI_DEVICE_ID_INTEL_C627_SUPER_1:
	case PCI_DEVICE_ID_INTEL_C621_SUPER:
	case PCI_DEVICE_ID_INTEL_C627_SUPER_2:
	case PCI_DEVICE_ID_INTEL_C628_SUPER:
	case PCI_DEVICE_ID_INTEL_DNV_LPC:
		p2sb = pci_get_dev(sb->access, 0, 0, 0x1f, 1);
		break;
	case PCI_DEVICE_ID_INTEL_APL_LPC:
	case PCI_DEVICE_ID_INTEL_GLK_LPC:
		p2sb = pci_get_dev(sb->access, 0, 0, 0x0d, 0);
		break;
	case PCI_DEVICE_ID_INTEL_H310:
	case PCI_DEVICE_ID_INTEL_H370:
	case PCI_DEVICE_ID_INTEL_Z390:
	case PCI_DEVICE_ID_INTEL_Q370:
	case PCI_DEVICE_ID_INTEL_B360:
	case PCI_DEVICE_ID_INTEL_C246:
	case PCI_DEVICE_ID_INTEL_C242:
	case PCI_DEVICE_ID_INTEL_QM370:
	case PCI_DEVICE_ID_INTEL_HM370:
	case PCI_DEVICE_ID_INTEL_CM246:
	case PCI_DEVICE_ID_INTEL_Q570:
	case PCI_DEVICE_ID_INTEL_Z590:
	case PCI_DEVICE_ID_INTEL_H570:
	case PCI_DEVICE_ID_INTEL_B560:
	case PCI_DEVICE_ID_INTEL_H510:
	case PCI_DEVICE_ID_INTEL_WM590:
	case PCI_DEVICE_ID_INTEL_QM580:
	case PCI_DEVICE_ID_INTEL_HM570:
	case PCI_DEVICE_ID_INTEL_C252:
	case PCI_DEVICE_ID_INTEL_C256:
	case PCI_DEVICE_ID_INTEL_W580:
	case 0x0660: case 0x0661: /* CML-U */
	case 0x0684: case 0x0685: case 0x0686: case 0x0687:
	case 0x0688: case 0x0689: case 0x068a: case 0x068b:
	case 0x068c: case 0x068d: case 0x068e: case 0x068f: /* CML-DT */
	case 0x7e00: case 0x7e01: case 0x7e02: case 0x7e03:
	case 0x7e04: case 0x7e05: case 0x7e06: case 0x7e07: /* MTL */
	case PCI_DEVICE_ID_INTEL_CANNONPOINT_LP_U_PREM:
	case 0x9d85: case 0x9d86: case 0x9d87: case 0x9d88:
	case 0x9d89: case 0x9d8a: case 0x9d8b: case 0x9d8c:
	case 0x9d8d: case 0x9d8e: case 0x9d8f: /* CNP-LP */
	case PCI_DEVICE_ID_INTEL_COMETPOINT_LP_U_PREM:
	case PCI_DEVICE_ID_INTEL_COMETPOINT_LP_U_BASE:
	case PCI_DEVICE_ID_INTEL_ICELAKE_LP_U:
	case PCI_DEVICE_ID_INTEL_TIGERPOINT_U_SUPER:
	case PCI_DEVICE_ID_INTEL_TIGERPOINT_U_PREM:
	case PCI_DEVICE_ID_INTEL_TIGERPOINT_U_BASE:
	case 0xa084: case 0xa085: /* TGL U missing */
	case PCI_DEVICE_ID_INTEL_TIGERPOINT_Y_SUPER:
	case PCI_DEVICE_ID_INTEL_TIGERPOINT_Y_PREM:
	case 0xa088: case 0xa089: case 0xa08a: case 0xa08b:
	case 0xa08c: case 0xa08d: case 0xa08e: case 0xa08f: /* TGL U/Y missing */
	case 0xa0a0: case 0xa0a1: case 0xa0a2: case 0xa0a3:
	case 0xa0a4: case 0xa0a5: case 0xa0a6: case 0xa0a7: /* TGL U/Y missing */
	case PCI_DEVICE_ID_INTEL_ADL_P:
	case PCI_DEVICE_ID_INTEL_ADL_M:
	case 0x5180: case 0x5181: case 0x5183: case 0x5184:
	case 0x5185: case 0x5186: case 0x5188: case 0x5189:
	case 0x518a: case 0x518b: case 0x518c: case 0x518d:
	case 0x518e: case 0x518f: /* ADL-P missing */
	case PCI_DEVICE_ID_INTEL_RPL_P:
	case 0x5190: case 0x5191: case 0x5192: case 0x5193:
	case 0x5194: case 0x5195: case 0x5196: case 0x5197:
	case 0x5198: case 0x5199: case 0x519a: case 0x519b:
	case 0x519c: case 0x519e: case 0x519f: /* RPL-P missing */
	case PCI_DEVICE_ID_INTEL_EHL:
	case PCI_DEVICE_ID_INTEL_JSL:
	case PCI_DEVICE_ID_INTEL_EBG:
		sbbar_phys = 0xfd000000;
		use_p2sb = false;
		break;
	case PCI_DEVICE_ID_INTEL_H610E:
	case PCI_DEVICE_ID_INTEL_Q670E:
	case PCI_DEVICE_ID_INTEL_R680E:
	case PCI_DEVICE_ID_INTEL_H610:
	case PCI_DEVICE_ID_INTEL_B660:
	case PCI_DEVICE_ID_INTEL_H670:
	case PCI_DEVICE_ID_INTEL_Q670:
	case PCI_DEVICE_ID_INTEL_Z690:
	case PCI_DEVICE_ID_INTEL_W680:
	case PCI_DEVICE_ID_INTEL_WM690:
	case PCI_DEVICE_ID_INTEL_HM670:
	case PCI_DEVICE_ID_INTEL_W790:
	case PCI_DEVICE_ID_INTEL_Z790:
	case PCI_DEVICE_ID_INTEL_H770:
	case PCI_DEVICE_ID_INTEL_B760:
	case PCI_DEVICE_ID_INTEL_HM770:
	case PCI_DEVICE_ID_INTEL_WM790:
	case PCI_DEVICE_ID_INTEL_C262:
	case PCI_DEVICE_ID_INTEL_C266:
	case 0x7a80: case 0x7a81: case 0x7a82: case 0x7a89:
	case 0x7a8b: /* ADL-S missing */
	case 0x7a0e: case 0x7a0f: case 0x7a10: case 0x7a11:
	case 0x7a12: case 0x7a15: case 0x7a16: case 0x7a17: /* RPL-S missing */
	case 0x7e20: case 0x7e21: case 0x7e22: case 0x7e23:
	case 0x7e24: case 0x7e25: case 0x7e26: case 0x7e27:
	case 0x7e28: case 0x7e29: case 0x7e2a: case 0x7e2b:
	case 0x7e2c: case 0x7e2d: case 0x7e2e: case 0x7e2f: /* ARL-S */
		sbbar_phys = 0xe0000000;
		use_p2sb = false;
		break;
	default:
		fprintf(stderr, "Unknown LPC device ID %04x -- not in pcr.c database.\nReport at https://github.com/tlaurion/tpm-gpio-fail/issues\n", sb->device_id);
		exit(1);
	}

	if (use_p2sb) {
		if (!p2sb) {
			perror("Can't allocate device node for P2SB.");
			exit(1);
		}

		/* do not fill bases here, libpci refuses to refill later */
		pci_fill_info(p2sb, PCI_FILL_IDENT);
		if (p2sb->vendor_id == 0xffff && p2sb->device_id == 0xffff) {
			printf("Trying to reveal Primary to Sideband Bridge "
			       "(P2SB),\nlet's hope the OS doesn't mind... ");
			/* Do not use pci_write_long(). Bytes
			   surrounding 0xe0 must be maintained. */
			pci_write_byte(p2sb, 0xe0 + 1, 0);

			pci_fill_info(p2sb, PCI_FILL_IDENT | PCI_FILL_RESCAN);
			if (p2sb->vendor_id != 0xffff ||
			    p2sb->device_id != 0xffff) {
				printf("done.\n");
				p2sb_revealed = true;
			} else {
				printf("failed.\n");
				exit(1);
			}
		}
		pci_fill_info(p2sb, PCI_FILL_BASES | PCI_FILL_CLASS);

		sbbar_phys = p2sb->base_addr[0] & ~0xfULL;
	}

	printf("SBREG_BAR = 0x%08"PRIx64" (MEM)\n\n", (uint64_t)sbbar_phys);
	sbbar = map_physical(sbbar_phys, SBBAR_SIZE);
	if (sbbar == NULL) {
		perror("Error mapping SBREG_BAR");
		error_exit = true;
	}

	if (use_p2sb) {
		if (p2sb_revealed) {
			printf("Hiding Primary to Sideband Bridge (P2SB).\n");
			pci_write_byte(p2sb, 0xe0 + 1, 1);
		}
		pci_free_dev(p2sb);
	}

	if (error_exit)
		exit(1);
}

void pcr_cleanup(void)
{
	if (sbbar)
		unmap_physical((void *)sbbar, SBBAR_SIZE);
}
