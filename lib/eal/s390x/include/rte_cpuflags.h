/* SPDX-License-Identifier: BSD-3-Clause
 * (c) Copyright IBM Corp. 2018, 2019
 */

#ifndef _RTE_CPUFLAGS_S390X_H_
#define _RTE_CPUFLAGS_S390X_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enumeration of all CPU features supported
 */
enum rte_cpu_flag_t {
	RTE_CPUFLAG_ESAN3 = 0,
	RTE_CPUFLAG_ZARCH,
	RTE_CPUFLAG_STFLE,
	RTE_CPUFLAG_MSA,
	RTE_CPUFLAG_LDISP,
	RTE_CPUFLAG_EIMM,
	RTE_CPUFLAG_DFP,
	RTE_CPUFLAG_HPAGE, //from elf.h
	//RTE_CPUFLAG_EDAT, //from hwcap.h
	RTE_CPUFLAG_ETF3EH,
	RTE_CPUFLAG_HIGH_GPRS,
	RTE_CPUFLAG_TE,
	RTE_CPUFLAG_VXRS,
	RTE_CPUFLAG_VXRS_BCD,
	RTE_CPUFLAG_VXRS_EXT,
	RTE_CPUFLAG_GS,
	/* The last item */
	RTE_CPUFLAG_NUMFLAGS,/**< This should always be the last! */
};

#include "generic/rte_cpuflags.h"

#ifdef __cplusplus
}
#endif

#endif /* _RTE_CPUFLAGS_S390X_H_ */
