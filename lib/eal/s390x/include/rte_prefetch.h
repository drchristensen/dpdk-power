/* SPDX-License-Identifier: BSD-3-Clause
 * (c) Copyright IBM Corp. 2018, 2019
 */

#ifndef _RTE_PREFETCH_S390X_H_
#define _RTE_PREFETCH_S390X_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <rte_common.h>
#include "generic/rte_prefetch.h"

static inline void rte_prefetch0(const volatile void *p)
{
	asm volatile ("pfd 1, 0(%[p])" : : [p] "r" (p));
}

static inline void rte_prefetch1(const volatile void *p)
{
	asm volatile ("pfd 1, 0(%[p])" : : [p] "r" (p));
}

static inline void rte_prefetch2(const volatile void *p)
{
	asm volatile ("pfd 1, 0(%[p])" : : [p] "r" (p));
}

static inline void rte_prefetch_non_temporal(const volatile void *p)
{
	/* non-temporal version not available, fallback to rte_prefetch0 */
	rte_prefetch0(p);
}

__rte_experimental
static inline void rte_cldemote(const volatile void *p)
{
    RTE_SET_USED(p);
}

#ifdef __cplusplus
}
#endif

#endif /* _RTE_PREFETCH_S390X_H_ */
