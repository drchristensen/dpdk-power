/* SPDX-License-Identifier: BSD-3-Clause
 * (c) Copyright IBM Corp. 2018, 2019
 */

#ifndef _RTE_CYCLES_S390X_H_
#define _RTE_CYCLES_S390X_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "generic/rte_cycles.h"

#include <rte_common.h>

/**
 * Read the time base register.
 *
 * @return
 *   The time base for this lcore.
 */
static inline uint64_t
rte_rdtsc(void)
{
	uint64_t tsc;
	asm volatile("stckf %0" : "=Q"(tsc) : : "cc");
	return tsc;
}

static inline uint64_t
rte_rdtsc_precise(void)
{
	rte_mb();
	return rte_rdtsc();
}

static inline uint64_t
rte_get_tsc_cycles(void) { return rte_rdtsc(); }

#ifdef __cplusplus
}
#endif

#endif /* _RTE_CYCLES_S390X_H_ */
