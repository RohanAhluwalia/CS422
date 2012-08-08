/*
 * Derived from BHyVe (svn 237539).
 * Adapted for CertiKOS by Haozhong Zhang at Yale.
 *
 * XXX: BHyVe is a 64-bit hypervisor, while CertiKOS is 32-bit.
 */

/*-
 * Copyright (c) 2011 NetApp, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NETAPP, INC ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL NETAPP, INC OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#ifndef _VIRT_VMX_VMCS_H_
#define _VIRT_VMX_VMCS_H_

#ifdef _KERN_

#include <sys/gcc.h>
#include <sys/mmu.h>
#include <sys/types.h>

struct vmcs {
	uint32_t	identifier;
	uint32_t	abort_code;
	char		_impl_specific[PAGE_SIZE - sizeof(uint32_t) * 2];
};

/* MSR save region is composed of an array of 'struct msr_entry' */
struct msr_entry {
	uint32_t	index;
	uint32_t	reserved;
	uint64_t	val;
};

uint16_t vmcs_read16(uint32_t encoding);
uint32_t vmcs_read32(uint32_t encoding);
uint64_t vmcs_read64(uint32_t encoding);

void vmcs_write16(uint32_t encoding, uint16_t val);
void vmcs_write32(uint32_t encoding, uint32_t val);
void vmcs_write64(uint32_t encoding, uint64_t val);

void vmcs_set_defaults(struct vmcs *,
		       uint64_t *pml4ept,       uint32_t pinbased_ctls,
		       uint32_t procbased_ctls, uint32_t procbased_ctls2,
		       uint32_t exit_ctls,      uint32_t entry_ctls,
		       char *msr_bitmap,
		       char *io_bitmap_a,       char *io_bitmap_b,
		       uint16_t vpid,
		       uint64_t cr0_ones_mask,  uint64_t cr0_zeros_mask,
		       uint64_t cr4_ones_mask,  uint64_t cr4_zeros_mask,
		       uintptr_t host_rip);

#define	VMCS_INITIAL			0xffffffffffffffff

#define	VMCS_IDENT(encoding)		((encoding) | 0x80000000)
/*
 * VMCS field encodings from Appendix H, Intel Architecture Manual Vol3B.
 */
#define	VMCS_INVALID_ENCODING		0xffffffff

/* 16-bit control fields */
#define	VMCS_VPID			0x00000000

/* 16-bit guest-state fields */
#define	VMCS_GUEST_ES_SELECTOR		0x00000800
#define	VMCS_GUEST_CS_SELECTOR		0x00000802
#define	VMCS_GUEST_SS_SELECTOR		0x00000804
#define	VMCS_GUEST_DS_SELECTOR		0x00000806
#define	VMCS_GUEST_FS_SELECTOR		0x00000808
#define	VMCS_GUEST_GS_SELECTOR		0x0000080A
#define	VMCS_GUEST_LDTR_SELECTOR	0x0000080C
#define	VMCS_GUEST_TR_SELECTOR		0x0000080E

/* 16-bit host-state fields */
#define	VMCS_HOST_ES_SELECTOR		0x00000C00
#define	VMCS_HOST_CS_SELECTOR		0x00000C02
#define	VMCS_HOST_SS_SELECTOR		0x00000C04
#define	VMCS_HOST_DS_SELECTOR		0x00000C06
#define	VMCS_HOST_FS_SELECTOR		0x00000C08
#define	VMCS_HOST_GS_SELECTOR		0x00000C0A
#define	VMCS_HOST_TR_SELECTOR		0x00000C0C

/* 64-bit control fields */
#define	VMCS_IO_BITMAP_A		0x00002000
#define	VMCS_IO_BITMAP_B		0x00002002
#define	VMCS_MSR_BITMAP			0x00002004
#define	VMCS_EXIT_MSR_STORE		0x00002006
#define	VMCS_EXIT_MSR_LOAD		0x00002008
#define	VMCS_ENTRY_MSR_LOAD		0x0000200A
#define	VMCS_EXECUTIVE_VMCS		0x0000200C
#define	VMCS_TSC_OFFSET			0x00002010
#define	VMCS_VIRTUAL_APIC		0x00002012
#define	VMCS_APIC_ACCESS		0x00002014
#define	VMCS_EPTP			0x0000201A

/* 64-bit read-only fields */
#define	VMCS_GUEST_PHYSICAL_ADDRESS	0x00002400

/* 64-bit guest-state fields */
#define	VMCS_LINK_POINTER		0x00002800
#define	VMCS_GUEST_IA32_DEBUGCTL	0x00002802
#define	VMCS_GUEST_IA32_PAT		0x00002804
#define	VMCS_GUEST_IA32_EFER		0x00002806
#define	VMCS_GUEST_IA32_PERF_GLOBAL_CTRL 0x00002808
#define	VMCS_GUEST_PDPTE0		0x0000280A
#define	VMCS_GUEST_PDPTE1		0x0000280C
#define	VMCS_GUEST_PDPTE2		0x0000280E
#define	VMCS_GUEST_PDPTE3		0x00002810

/* 64-bit host-state fields */
#define	VMCS_HOST_IA32_PAT		0x00002C00
#define	VMCS_HOST_IA32_EFER		0x00002C02
#define	VMCS_HOST_IA32_PERF_GLOBAL_CTRL	0x00002C04

/* 32-bit control fields */
#define	VMCS_PIN_BASED_CTLS		0x00004000
#define	VMCS_PRI_PROC_BASED_CTLS	0x00004002
#define	VMCS_EXCEPTION_BITMAP		0x00004004
#define	VMCS_PF_ERROR_MASK		0x00004006
#define	VMCS_PF_ERROR_MATCH		0x00004008
#define	VMCS_CR3_TARGET_COUNT		0x0000400A
#define	VMCS_EXIT_CTLS			0x0000400C
#define	VMCS_EXIT_MSR_STORE_COUNT	0x0000400E
#define	VMCS_EXIT_MSR_LOAD_COUNT	0x00004010
#define	VMCS_ENTRY_CTLS			0x00004012
#define	VMCS_ENTRY_MSR_LOAD_COUNT	0x00004014
#define	VMCS_ENTRY_INTR_INFO		0x00004016
#define	VMCS_ENTRY_EXCEPTION_ERROR	0x00004018
#define	VMCS_ENTRY_INST_LENGTH		0x0000401A
#define	VMCS_TPR_THRESHOLD		0x0000401C
#define	VMCS_SEC_PROC_BASED_CTLS	0x0000401E
#define	VMCS_PLE_GAP			0x00004020
#define	VMCS_PLE_WINDOW			0x00004022

/* 32-bit read-only data fields */
#define	VMCS_INSTRUCTION_ERROR		0x00004400
#define	VMCS_EXIT_REASON		0x00004402
#define	VMCS_EXIT_INTERRUPTION_INFO	0x00004404
#define	VMCS_EXIT_INTERRUPTION_ERROR	0x00004406
#define	VMCS_IDT_VECTORING_INFO		0x00004408
#define	VMCS_IDT_VECTORING_ERROR	0x0000440A
#define	VMCS_EXIT_INSTRUCTION_LENGTH	0x0000440C
#define	VMCS_EXIT_INSTRUCTION_INFO	0x0000440E

/* 32-bit guest-state fields */
#define	VMCS_GUEST_ES_LIMIT		0x00004800
#define	VMCS_GUEST_CS_LIMIT		0x00004802
#define	VMCS_GUEST_SS_LIMIT		0x00004804
#define	VMCS_GUEST_DS_LIMIT		0x00004806
#define	VMCS_GUEST_FS_LIMIT		0x00004808
#define	VMCS_GUEST_GS_LIMIT		0x0000480A
#define	VMCS_GUEST_LDTR_LIMIT		0x0000480C
#define	VMCS_GUEST_TR_LIMIT		0x0000480E
#define	VMCS_GUEST_GDTR_LIMIT		0x00004810
#define	VMCS_GUEST_IDTR_LIMIT		0x00004812
#define	VMCS_GUEST_ES_ACCESS_RIGHTS	0x00004814
#define	VMCS_GUEST_CS_ACCESS_RIGHTS	0x00004816
#define	VMCS_GUEST_SS_ACCESS_RIGHTS	0x00004818
#define	VMCS_GUEST_DS_ACCESS_RIGHTS	0x0000481A
#define	VMCS_GUEST_FS_ACCESS_RIGHTS	0x0000481C
#define	VMCS_GUEST_GS_ACCESS_RIGHTS	0x0000481E
#define	VMCS_GUEST_LDTR_ACCESS_RIGHTS	0x00004820
#define	VMCS_GUEST_TR_ACCESS_RIGHTS	0x00004822
#define	VMCS_GUEST_INTERRUPTIBILITY	0x00004824
#define	VMCS_GUEST_ACTIVITY		0x00004826
#define VMCS_GUEST_SMBASE		0x00004828
#define	VMCS_GUEST_IA32_SYSENTER_CS	0x0000482A
#define	VMCS_PREEMPTION_TIMER_VALUE	0x0000482E

/* 32-bit host state fields */
#define	VMCS_HOST_IA32_SYSENTER_CS	0x00004C00

/* Natural Width control fields */
#define	VMCS_CR0_MASK			0x00006000
#define	VMCS_CR4_MASK			0x00006002
#define	VMCS_CR0_SHADOW			0x00006004
#define	VMCS_CR4_SHADOW			0x00006006
#define	VMCS_CR3_TARGET0		0x00006008
#define	VMCS_CR3_TARGET1		0x0000600A
#define	VMCS_CR3_TARGET2		0x0000600C
#define	VMCS_CR3_TARGET3		0x0000600E

/* Natural Width read-only fields */
#define	VMCS_EXIT_QUALIFICATION		0x00006400
#define	VMCS_IO_RCX			0x00006402
#define	VMCS_IO_RSI			0x00006404
#define	VMCS_IO_RDI			0x00006406
#define	VMCS_IO_RIP			0x00006408
#define	VMCS_GUEST_LINEAR_ADDRESS	0x0000640A

/* Natural Width guest-state fields */
#define	VMCS_GUEST_CR0			0x00006800
#define	VMCS_GUEST_CR3			0x00006802
#define	VMCS_GUEST_CR4			0x00006804
#define	VMCS_GUEST_ES_BASE		0x00006806
#define	VMCS_GUEST_CS_BASE		0x00006808
#define	VMCS_GUEST_SS_BASE		0x0000680A
#define	VMCS_GUEST_DS_BASE		0x0000680C
#define	VMCS_GUEST_FS_BASE		0x0000680E
#define	VMCS_GUEST_GS_BASE		0x00006810
#define	VMCS_GUEST_LDTR_BASE		0x00006812
#define	VMCS_GUEST_TR_BASE		0x00006814
#define	VMCS_GUEST_GDTR_BASE		0x00006816
#define	VMCS_GUEST_IDTR_BASE		0x00006818
#define	VMCS_GUEST_DR7			0x0000681A
#define	VMCS_GUEST_RSP			0x0000681C
#define	VMCS_GUEST_RIP			0x0000681E
#define	VMCS_GUEST_RFLAGS		0x00006820
#define	VMCS_GUEST_PENDING_DBG_EXCEPTIONS 0x00006822
#define	VMCS_GUEST_IA32_SYSENTER_ESP	0x00006824
#define	VMCS_GUEST_IA32_SYSENTER_EIP	0x00006826

/* Natural Width host-state fields */
#define	VMCS_HOST_CR0			0x00006C00
#define	VMCS_HOST_CR3			0x00006C02
#define	VMCS_HOST_CR4			0x00006C04
#define	VMCS_HOST_FS_BASE		0x00006C06
#define	VMCS_HOST_GS_BASE		0x00006C08
#define	VMCS_HOST_TR_BASE		0x00006C0A
#define	VMCS_HOST_GDTR_BASE		0x00006C0C
#define	VMCS_HOST_IDTR_BASE		0x00006C0E
#define	VMCS_HOST_IA32_SYSENTER_ESP	0x00006C10
#define	VMCS_HOST_IA32_SYSENTER_EIP	0x00006C12
#define	VMCS_HOST_RSP			0x00006C14
#define	VMCS_HOST_RIP			0x00006c16

/*
 * VM instruction error numbers
 */
#define	VMRESUME_WITH_NON_LAUNCHED_VMCS	5

/* VMCS_EXIT_REASON fields */
#define EXIT_REASON_ENTRY_FAIL		(1 << 31)
#define EXIT_REASON_FROM_ROOT		(1 << 29)
#define EXIT_REASON_PENDING_MFT		(1 << 28)
#define EXIT_REASON_MASK		0x0000ffff

/*
 * VMCS exit reasons
 */
#define EXIT_REASON_EXCEPTION		0
#define EXIT_REASON_EXT_INTR		1
#define EXIT_REASON_TRIPLE_FAULT	2
#define EXIT_REASON_INIT		3
#define EXIT_REASON_SIPI		4
#define EXIT_REASON_IO_SMI		5
#define EXIT_REASON_SMI			6
#define EXIT_REASON_INTR_WINDOW		7
#define EXIT_REASON_NMI_WINDOW		8
#define EXIT_REASON_TASK_SWITCH		9
#define EXIT_REASON_CPUID		10
#define EXIT_REASON_GETSEC		11
#define EXIT_REASON_HLT			12
#define EXIT_REASON_INVD		13
#define EXIT_REASON_INVLPG		14
#define EXIT_REASON_RDPMC		15
#define EXIT_REASON_RDTSC		16
#define EXIT_REASON_RSM			17
#define EXIT_REASON_VMCALL		18
#define EXIT_REASON_VMCLEAR		19
#define EXIT_REASON_VMLAUNCH		20
#define EXIT_REASON_VMPTRLD		21
#define EXIT_REASON_VMPTRST		22
#define EXIT_REASON_VMREAD		23
#define EXIT_REASON_VMRESUME		24
#define EXIT_REASON_VMWRITE		25
#define EXIT_REASON_VMXOFF		26
#define EXIT_REASON_VMXON		27
#define EXIT_REASON_CR_ACCESS		28
#define EXIT_REASON_DR_ACCESS		29
#define EXIT_REASON_INOUT		30
#define EXIT_REASON_RDMSR		31
#define EXIT_REASON_WRMSR		32
#define EXIT_REASON_INVAL_VMCS		33
#define EXIT_REASON_INVAL_MSR		34
#define EXIT_REASON_MWAIT		36
#define EXIT_REASON_MTF			37
#define EXIT_REASON_MONITOR		39
#define EXIT_REASON_PAUSE		40
#define EXIT_REASON_MCE			41
#define EXIT_REASON_TPR			43
#define EXIT_REASON_APIC		44
#define EXIT_REASON_GDTR_IDTR		46
#define EXIT_REASON_LDTR_TR		47
#define EXIT_REASON_EPT_FAULT		48
#define EXIT_REASON_EPT_MISCONFIG	49
#define EXIT_REASON_INVEPT		50
#define EXIT_REASON_RDTSCP		51
#define EXIT_REASON_VMX_PREEMPT		52
#define EXIT_REASON_INVVPID		53
#define EXIT_REASON_WBINVD		54
#define EXIT_REASON_XSETBV		55
#define EXIT_REASON_RDRAND		57
#define EXIT_REASON_INVPCID		58
#define EXIT_REASON_VMFUNC		59

/*
 * VMCS interrupt information fields
 */
#define	VMCS_INTERRUPTION_INFO_VALID	(1U << 31)
#define	VMCS_INTERRUPTION_INFO_HW_INTR	(0 << 8)
#define	VMCS_INTERRUPTION_INFO_NMI	(2 << 8)
#define VMCS_INTERRUPTION_INFO_HW_EXCPT	(3 << 8)

/*
 * VMCS Guest interruptibility field
 */
#define	VMCS_INTERRUPTIBILITY_STI_BLOCKING	(1 << 0)
#define	VMCS_INTERRUPTIBILITY_MOVSS_BLOCKING	(1 << 1)
#define	VMCS_INTERRUPTIBILITY_SMI_BLOCKING	(1 << 2)
#define	VMCS_INTERRUPTIBILITY_NMI_BLOCKING	(1 << 3)

/*
 * Exit qualification for EXIT_REASON_INVAL_VMCS
 */
#define	EXIT_QUAL_NMI_WHILE_STI_BLOCKING	3

/*
 * Exit qualification for I/O instructions
 */
#define EXIT_QUAL_IO_SIZE(q)	((q) & 7)
# define EXIT_QUAL_IO_ONE_BYTE	0
# define EXIT_QUAL_IO_TWO_BYTE	1
# define EXIT_QUAL_IO_FOUR_BYTE	3
#define EXIT_QUAL_IO_DIR(q)	(((q) >> 3) & 1)
# define EXIT_QUAL_IO_OUT	0
# define EXIT_QUAL_IO_IN	1
#define EXIT_QUAL_IO_STR(q)	(((q) >> 4) & 1)
#define EXIT_QUAL_IO_REP(q)	(((q) >> 5) & 1)
#define EXIT_QUAL_IO_IMM(q)	(((q) >> 6) & 1)
#define EXIT_QUAL_IO_PORT(q)	((uint16_t) (((q) >> 16) & 0xffff))

#endif /* _KERN_ */

#endif /* !_VIRT_VMX_VMCS_H_ */
