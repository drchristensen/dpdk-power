/* SPDX-License-Identifier: BSD-3-Clause
 * (c) Copyright IBM Corp. 2018, 2019
 */

#ifndef _RTE_MEMCPY_S390X_H_
#define _RTE_MEMCPY_S390X_H_

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "generic/rte_memcpy.h"


static inline void
rte_mov16(uint8_t *dst, const uint8_t *src)
{
	memcpy(dst, src, 16);
}
static inline void
rte_mov32(uint8_t *dst, const uint8_t *src)
{
	memcpy(dst, src, 32);
}
static inline void
rte_mov48(uint8_t *dst, const uint8_t *src)
{
	memcpy(dst, src, 48);
}
static inline void
rte_mov64(uint8_t *dst, const uint8_t *src)
{
	memcpy(dst, src, 64);
}
static inline void
rte_mov128(uint8_t *dst, const uint8_t *src)
{
	memcpy(dst, src, 128);
}
static inline void
rte_mov256(uint8_t *dst, const uint8_t *src)
{
	memcpy(dst, src, 256);
}
#define rte_memcpy(d, s, n)	memcpy((d), (s), (n))


#ifdef __cplusplus
}
#endif

#endif /* _RTE_MEMCPY_S390X_H_ */
