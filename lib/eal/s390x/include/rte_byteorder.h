/* SPDX-License-Identifier: BSD-3-Clause
 * (c) Copyright IBM Corp. 2018, 2019
 */

/* Inspired from FreeBSD src/sys/powerpc/include/endian.h
 * Copyright (c) 1987, 1991, 1993
 * The Regents of the University of California.  All rights reserved.
 */

#ifndef _RTE_BYTEORDER_S390X_H_
#define _RTE_BYTEORDER_S390X_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "generic/rte_byteorder.h"

/* s390x is big endian
 */

#define rte_cpu_to_le_16(x) rte_bswap16(x)
#define rte_cpu_to_le_32(x) rte_bswap32(x)
#define rte_cpu_to_le_64(x) rte_bswap64(x)

#define rte_cpu_to_be_16(x) (x)
#define rte_cpu_to_be_32(x) (x)
#define rte_cpu_to_be_64(x) (x)

#define rte_le_to_cpu_16(x) rte_bswap16(x)
#define rte_le_to_cpu_32(x) rte_bswap32(x)
#define rte_le_to_cpu_64(x) rte_bswap64(x)

#define rte_be_to_cpu_16(x) (x)
#define rte_be_to_cpu_32(x) (x)
#define rte_be_to_cpu_64(x) (x)

#ifdef __cplusplus
}
#endif

#endif /* _RTE_BYTEORDER_S390X_H_ */
