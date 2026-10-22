/* inteltool - dump all registers on an Intel CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>
#include <unistd.h>
#include "pcr.h"

const uint8_t *sbbar = NULL;
bool pcr_use_p2sb = false;

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
	case PCI_DEVICE_ID_INTEL_GLK_ESPI:
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
	case 0x0281: case 0x0283: case 0x0286: /* CML-U/Y (remaining; 0x0284/0x0285 below) */
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
	case PCI_DEVICE_ID_INTEL_ADL_P:
	case PCI_DEVICE_ID_INTEL_ADL_M:
	case 0x5180: case 0x5181: case 0x5183: case 0x5184:
	case 0x5185: case 0x5186: case 0x5188: case 0x5189:
	case 0x518a: case 0x518b: case 0x518c: case 0x518d:
	case 0x518e: case 0x518f: /* ADL-P missing */
	case PCI_DEVICE_ID_INTEL_ADL_N:
	case PCI_DEVICE_ID_INTEL_ADL_N_2:
	case PCI_DEVICE_ID_INTEL_RPL_P:
	case 0x5190: case 0x5191: case 0x5192: case 0x5193:
	case 0x5194: case 0x5195: case 0x5196: case 0x5197:
	case 0x5198: case 0x5199: case 0x519a: case 0x519b:
	case 0x519c: case 0x519e: case 0x519f: /* RPL-P missing */
	case PCI_DEVICE_ID_INTEL_EHL: /* 0x4b00 */
	case 0x4b01: case 0x4b02: case 0x4b03:
	case 0x4b04: case 0x4b05: case 0x4b06: case 0x4b07: /* EHL other SKUs */
	case PCI_DEVICE_ID_INTEL_JSL:
	case PCI_DEVICE_ID_INTEL_EBG:
	case 0x0684: case 0x0685: case 0x0686: case 0x0687:
	case 0x0688: case 0x0689: case 0x068a: case 0x068b:
	case 0x068c: case 0x068d: case 0x068e: case 0x068f:
	case 0x0697: /* CML-DT/H (desktop, 0xfd000000) */
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
	case 0x7e00: case 0x7e01: case 0x7e02: case 0x7e03:
	case 0x7e04: case 0x7e05: case 0x7e06: case 0x7e07: /* MTL (0xe0000000, not 0xfd000000) */
		sbbar_phys = 0xe0000000;
		use_p2sb = false;
		break;
	default:
		if (sb->device_id == 0x2918 || sb->device_id == 0x7010)
			fprintf(stderr, "LPC device 0x%04x is a QEMU emulated LPC bridge (ICH9/PIIX4) -- emulated platform, no physical Intel PCH\n", sb->device_id);
		else
			fprintf(stderr, "Unknown LPC device ID %04x -- not in pcr.c database.\nReport at https://github.com/tlaurion/tpm-gpio-fail/issues\n", sb->device_id);
		exit(126);
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
	pcr_use_p2sb = use_p2sb;
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

static uint16_t pcr_sbi_status_read(struct pci_dev *p2sb)
{
	return pci_read_word(p2sb, P2SB_CR_SBI_STATUS);
}

/* Poll P2SB_CR_SBI_STATUS[0] (busy) until the previous sideband op completes. */
static int pcr_sbi_wait_idle(struct pci_dev *p2sb)
{
	int i;

	for (i = 0; i < P2SB_SBI_TIMEOUT_POLLS; i++) {
		if (!(pcr_sbi_status_read(p2sb) & P2SB_CR_SBI_STATUS_BUSY))
			return 0;
		usleep(100); /* 100us */
	}
	return -1;
}

/* Execute one non-posted P2SB sideband message (write opcode).
 * Mirrors coreboot's pcr_execute_sideband_msg(). */
static int pcr_execute_sideband_msg(struct pci_dev *p2sb,
				    const struct pcr_sbi_msg *msg,
				    uint32_t *data, uint8_t *response)
{
	uint16_t sbi_status;
	uint16_t sbi_rid;
	int i;

	if (!msg || !data || !response)
		return -1;

	/* Vendor ID at offset 0; 0xffff means the device is hidden. */
	if (pci_read_word(p2sb, 0) == 0xffff) {
		fprintf(stderr, "SBI Failure: P2SB device Hidden!\n");
		return -1;
	}

	/* 1. Wait until the previous operation completed (STATUS[0] == 0). */
	if (pcr_sbi_wait_idle(p2sb)) {
		fprintf(stderr, "SBI Failure: Time Out!\n");
		return -1;
	}

	*response = P2SB_CR_SBI_STATUS_NOT_SUPPORTED;

	/* 2. Write ADDR = (pid << 24) | offset. */
	pci_write_long(p2sb, P2SB_CR_SBI_ADDR,
		       (msg->pid << P2SB_CR_SBI_DESTID_SHIFT) | msg->offset);

	/* 3. Write extended address (high bits of offset). */
	pci_write_long(p2sb, P2SB_CR_SBI_EXT_ADDR, msg->offset >> 16);

	/* 4. Set STATUS[15:8] = opcode, STATUS[7] = posted (0 = non-posted). */
	sbi_status = pci_read_word(p2sb, P2SB_CR_SBI_STATUS);
	sbi_status &= ~(P2SB_CR_SBI_OPCODE_MASK | P2SB_CR_SBI_POSTED_MASK);
	sbi_status |= (msg->opcode << P2SB_CR_SBI_OPCODE_SHIFT) |
		      (msg->is_posted << P2SB_CR_SBI_POSTED_SHIFT);
	pci_write_word(p2sb, P2SB_CR_SBI_STATUS, sbi_status);

	/* 5. Write RIDEN = (FBE << 12) | (BAR << 8) | FID. */
	sbi_rid = ((msg->fast_byte_enable & 0xf) << P2SB_CR_SBI_FBE_SHIFT) |
		  ((msg->bar & 0x7) << P2SB_CR_SBI_BAR_SHIFT) |
		  (msg->fid & 0xff);
	pci_write_word(p2sb, P2SB_CR_SBI_ROUTE_IDEN, sbi_rid);

	/* 6. Write DATA. */
	pci_write_long(p2sb, P2SB_CR_SBI_DATA, *data);

	/* 7. Trigger: set STATUS[0] = 1 (busy). */
	sbi_status = pci_read_word(p2sb, P2SB_CR_SBI_STATUS);
	sbi_status |= P2SB_CR_SBI_STATUS_BUSY;
	pci_write_word(p2sb, P2SB_CR_SBI_STATUS, sbi_status);

	/* 8. Poll until STATUS[0] == 0 again. */
	if (pcr_sbi_wait_idle(p2sb)) {
		fprintf(stderr, "SBI Failure: Time Out!\n");
		return -1;
	}

	/* 9. Transaction status = STATUS[2:1]. */
	sbi_status = pci_read_word(p2sb, P2SB_CR_SBI_STATUS);
	*response = (sbi_status & P2SB_CR_SBI_STATUS_MASK) >> P2SB_CR_SBI_STATUS_SHIFT;
	if (*response == P2SB_CR_SBI_STATUS_SUCCESS)
		return 0;

	fprintf(stderr, "SBI Failure: Transaction Status = %d\n", *response);
	return -1;
}

/* Clear (unlock) then restore PADCFGLOCK/PADCFGLOCKTX via non-posted SBI.
 * dw0: offset0/bit0  dw1: offset1/bit1 (offset1 = offset0 + 4).
 * val0/val1 are the full register values to write (clear = orig & ~bit,
 * restore = orig). Returns 0 on success. */
int pcr_lock_regs_sbi(struct pci_dev *const sb, const uint8_t pid,
		      const uint16_t offset0, const uint32_t val0,
		      const uint16_t offset1, const uint32_t val1)
{
	struct pci_dev *p2sb;
	struct pcr_sbi_msg msg;
	uint32_t data;
	uint8_t response;
	int status = -1;
	bool revealed = false;

	p2sb = pci_get_dev(sb->access, 0, 0, 0x1f, 1);
	if (!p2sb) {
		fprintf(stderr, "Can't allocate P2SB device node.\n");
		return -1;
	}

	pci_fill_info(p2sb, PCI_FILL_IDENT);
	if (p2sb->vendor_id == 0xffff && p2sb->device_id == 0xffff) {
		fprintf(stderr, "Revealing hidden P2SB ... ");
		/* Do not use pci_write_long(): bytes surrounding 0xe0
		   must be maintained. */
		pci_write_byte(p2sb, P2SB_CR_SBI_P2SB_REVEAL +
			       P2SB_CR_SBI_P2SB_REVEAL_BYTE, 0);
		pci_fill_info(p2sb, PCI_FILL_IDENT | PCI_FILL_RESCAN);
		if (p2sb->vendor_id == 0xffff && p2sb->device_id == 0xffff) {
			fprintf(stderr, "failed.\n");
			pci_free_dev(p2sb);
			return -1;
		}
		fprintf(stderr, "done.\n");
		revealed = true;
	}
	pci_fill_info(p2sb, PCI_FILL_BASES | PCI_FILL_CLASS);

	msg.pid = pid;
	msg.opcode = GPIO_LOCK_UNLOCK;
	msg.is_posted = false;
	msg.fast_byte_enable = 0xf;
	msg.bar = 0;
	msg.fid = 0;

	/* DW0: PADCFGLOCK. */
	msg.offset = offset0;
	data = val0;
	if (pcr_execute_sideband_msg(p2sb, &msg, &data, &response)) {
		fprintf(stderr, "SBI lock write DW0 (offset 0x%04x) failed\n",
			offset0);
		goto out;
	}

	/* DW1: PADCFGLOCKTX. */
	msg.offset = offset1;
	data = val1;
	if (pcr_execute_sideband_msg(p2sb, &msg, &data, &response)) {
		fprintf(stderr, "SBI lock write DW1 (offset 0x%04x) failed\n",
			offset1);
		goto out;
	}

	status = 0;

out:
	if (revealed) {
		fprintf(stderr, "Hiding P2SB.\n");
		pci_write_byte(p2sb, P2SB_CR_SBI_P2SB_REVEAL +
			       P2SB_CR_SBI_P2SB_REVEAL_BYTE, 1);
	}
	pci_free_dev(p2sb);
	return status;
}
