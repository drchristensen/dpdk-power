/* SPDX-License-Identifier: BSD-3-Clause
 * (c) Copyright IBM Corp. 2018, 2019
 */

#ifndef _RTE_VECT_S390X_H_
#define _RTE_VECT_S390X_H_

#include <vecintrin.h>
#include "generic/rte_vect.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RTE_VECT_DEFAULT_SIMD_BITWIDTH RTE_VECT_SIMD_256

typedef int xmm_t __attribute__((vector_size(4*sizeof(int))));

#define	XMM_SIZE	(sizeof(xmm_t))
#define	XMM_MASK	(XMM_SIZE - 1)

typedef union rte_xmm {
	xmm_t    x;
	uint8_t  u8[XMM_SIZE / sizeof(uint8_t)];
	uint16_t u16[XMM_SIZE / sizeof(uint16_t)];
	uint32_t u32[XMM_SIZE / sizeof(uint32_t)];
	uint64_t u64[XMM_SIZE / sizeof(uint64_t)];
	double   pd[XMM_SIZE / sizeof(double)];
} __attribute__((aligned(16))) rte_xmm_t;

#ifdef __cplusplus
}
#endif

#endif /* _RTE_VECT_S390X_H_ */
