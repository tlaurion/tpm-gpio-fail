/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef INTELTOOL_PCR_H
#define INTELTOOL_PCR_H 1

#include <stdint.h>
#include <stdbool.h>
#include "inteltool.h"

#define SBBAR_SIZE	(16 * MiB)
#define PCR_PORT_SIZE	(64 * KiB)

uint32_t read_pcr32(uint8_t port, uint16_t offset);
void write_pcr32(uint8_t port, uint16_t offset, uint32_t value);

void print_pcr_ports(struct pci_dev *sb, const uint8_t *ports, size_t count);

void pcr_init(struct pci_dev *sb);
void pcr_cleanup(void);

/* Primary-to-Sideband Bridge (P2SB) sideband message interface.
 * Pre-ADL PCH Pad Configuration Lock can only be set/cleared via a
 * non-posted sideband write; plain MMIO (write_pcr32) is a no-op there.
 * Mirrors soc/intel/common/block/pcr/pcr.c + include/intelblocks/pcr.h. */
struct pcr_sbi_msg {
	uint8_t pid;
	uint16_t offset;
	uint8_t opcode;
	bool is_posted;
	uint16_t fast_byte_enable;
	uint16_t bar;
	uint16_t fid;
};

/* P2SB PCI configuration registers */
#define P2SB_CR_SBI_ADDR		0xd0
#define P2SB_CR_SBI_DATA		0xd4
#define P2SB_CR_SBI_STATUS		0xd8
#define P2SB_CR_SBI_EXT_ADDR		0xdc
#define P2SB_CR_SBI_ROUTE_IDEN		0xda
#define P2SB_CR_SBI_P2SB_REVEAL		0xe0
#define P2SB_CR_SBI_P2SB_REVEAL_BYTE	1

/* P2SB_CR_SBI_ADDR bit 31:24 = Destination Port ID */
#define P2SB_CR_SBI_DESTID_SHIFT	24
/* P2SB_CR_SBI_STATUS bit 15:8 = opcode */
#define P2SB_CR_SBI_OPCODE_SHIFT	8
#define P2SB_CR_SBI_OPCODE_MASK		0xff00
/* bit 7 = posted */
#define P2SB_CR_SBI_POSTED_SHIFT	7
#define P2SB_CR_SBI_POSTED_MASK		0x0080
/* bits 2:1 = transaction status, bit 0 = busy */
#define P2SB_CR_SBI_STATUS_SHIFT	1
#define P2SB_CR_SBI_STATUS_MASK		0x0006
#define P2SB_CR_SBI_STATUS_READY	0
#define P2SB_CR_SBI_STATUS_SUCCESS	0
#define P2SB_CR_SBI_STATUS_NOT_SUPPORTED	1
#define P2SB_CR_SBI_STATUS_POWERED_DOWN	2
#define P2SB_CR_SBI_STATUS_BUSY		1
/* P2SB_CR_SBI_ROUTE_IDEN bits 15:12 = FBE, 10:8 = BAR, 7:0 = FID */
#define P2SB_CR_SBI_FBE_SHIFT		12
#define P2SB_CR_SBI_BAR_SHIFT		8
#define P2SB_CR_SBI_FID_SHIFT		0

#define GPIO_LOCK_UNLOCK	0x13
#define P2SB_SBI_TIMEOUT_POLLS	10000

/* True when pcr_init() reached SBREG_BAR through a revealed P2SB
 * (SKL/KBL/CNL/CML class). False when SBREG_BAR is a fixed MMIO bar
 * (ADL/MTL class). SBI lock writes only apply on the P2SB path. */
extern bool pcr_use_p2sb;

/* Clear (unlock) PADCFGLOCK DW0 at offset0/bit0 and PADCFGLOCKTX DW1 at
 * offset1/bit1 via a non-posted SBI GPIO_LOCK_UNLOCK message, then restore
 * them to val0/val1. Returns 0 on success. */
int pcr_lock_regs_sbi(struct pci_dev *sb, uint8_t pid,
		      uint16_t offset0, uint32_t val0,
		      uint16_t offset1, uint32_t val1);

#endif
