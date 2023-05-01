/* SPDX-License-Identifier: BSD-3-Clause
 * (c) Copyright IBM Corp. 2018, 2019
 */

#ifndef _RTE_ATOMIC_S390X_H_
#define _RTE_ATOMIC_S390X_H_

#ifndef RTE_FORCE_INTRINSICS
#  error Platform must be built with CONFIG_RTE_FORCE_INTRINSICS
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "generic/rte_atomic.h"

//#define dsb(opt) asm volatile("" : : : "memory")
//#define dmb(opt) asm volatile("" : : : "memory")

#define rte_mb() rte_compiler_barrier() //asm volatile("" : : : "memory")

#define rte_wmb() rte_mb()

#define rte_rmb() rte_mb()

#define rte_smp_mb() rte_mb()

#define rte_smp_wmb() rte_wmb()

#define rte_smp_rmb() rte_rmb()

#define rte_io_mb() rte_mb()

#define rte_io_wmb() rte_wmb()

#define rte_io_rmb() rte_rmb()

#define rte_cio_wmb() rte_wmb()

#define rte_cio_rmb() rte_rmb()

#ifdef __cplusplus
}
#endif

#endif /* _RTE_ATOMIC_S390X_H_ */
