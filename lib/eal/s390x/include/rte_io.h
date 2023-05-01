/* SPDX-License-Identifier: BSD-3-Clause
 * (c) Copyright IBM Corp. 2018, 2019
 */

#ifndef _RTE_IO_S390X_H_
#define _RTE_IO_S390X_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define RTE_OVERRIDE_IO_H

#include "generic/rte_io.h"

#include <unistd.h>
#include <sys/syscall.h>

union register_pair {
	__int128_t pair;
	struct {
		unsigned long even;
		unsigned long odd;
	} even_odd;
};

/* s390 requires special instructions to access IO memory. */
static inline uint64_t pcilgi(const volatile void *ioaddr, size_t len)
{
        union register_pair ioaddr_len =
                {.even_odd.even = (uint64_t)ioaddr, .even_odd.odd = len};
	uint64_t val;
	int cc = -1;

	asm volatile (
		"       .insn   rre,0xb9d60000,%[val],%[ioaddr_len]\n"
		"       ipm     %[cc]\n"
		"       srl     %[cc],28\n"
		: [cc] "+d" (cc), [val] "=d" (val),
		  [ioaddr_len] "+&d" (ioaddr_len.pair) :: "cc");
	return val;
}

static inline void pcistgi(volatile void *ioaddr, uint64_t val, size_t len)
{
        union register_pair ioaddr_len =
                {.even_odd.even = (uint64_t)ioaddr, .even_odd.odd = len};
	int cc = -1;

	asm volatile (
		"       .insn   rre,0xb9d40000,%[val],%[ioaddr_len]\n"
		"       ipm     %[cc]\n"
		"       srl     %[cc],28\n"
		: [cc] "+d" (cc), [ioaddr_len] "+&d" (ioaddr_len.pair)
		: [val] "d" (val)
		: "cc", "memory");
}

/* fall back to syscall on old machines ? */
static __rte_always_inline uint8_t
rte_read8_relaxed(const volatile void *addr)
{
	return pcilgi(addr, 1);
}

static __rte_always_inline uint16_t
rte_read16_relaxed(const volatile void *addr)
{
	return pcilgi(addr, 2);
}

static __rte_always_inline uint32_t
rte_read32_relaxed(const volatile void *addr)
{
	return pcilgi(addr, 4);
}

static __rte_always_inline uint64_t
rte_read64_relaxed(const volatile void *addr)
{
	return pcilgi(addr, 8);
}

static __rte_always_inline void
rte_write8_relaxed(uint8_t value, volatile void *addr)
{
	pcistgi(addr, value, sizeof(value));
}

static __rte_always_inline void
rte_write16_relaxed(uint16_t value, volatile void *addr)
{
	pcistgi(addr, value, sizeof(value));
}

static __rte_always_inline void
rte_write32_relaxed(uint32_t value, volatile void *addr)
{
	pcistgi(addr, value, sizeof(value));
}

static __rte_always_inline void
rte_write64_relaxed(uint64_t value, volatile void *addr)
{
	pcistgi(addr, value, sizeof(value));
}

static __rte_always_inline uint8_t
rte_read8(const volatile void *addr)
{
	uint8_t val;
	val = rte_read8_relaxed(addr);
	rte_io_rmb();
	return val;
}

static __rte_always_inline uint16_t
rte_read16(const volatile void *addr)
{
	uint16_t val;
	val = rte_read16_relaxed(addr);
	rte_io_rmb();
	return val;
}

static __rte_always_inline uint32_t
rte_read32(const volatile void *addr)
{
	uint32_t val;
	val = rte_read32_relaxed(addr);
	rte_io_rmb();
	return val;
}

static __rte_always_inline uint64_t
rte_read64(const volatile void *addr)
{
	uint64_t val;
	val = rte_read64_relaxed(addr);
	rte_io_rmb();
	return val;
}

static __rte_always_inline void
rte_write8(uint8_t value, volatile void *addr)
{
	rte_io_wmb();
	rte_write8_relaxed(value, addr);
}

static __rte_always_inline void
rte_write16(uint16_t value, volatile void *addr)
{
	rte_io_wmb();
	rte_write16_relaxed(value, addr);
}

static __rte_always_inline void
rte_write32(uint32_t value, volatile void *addr)
{
	rte_io_wmb();
	rte_write32_relaxed(value, addr);
}

static __rte_always_inline void
rte_write32_wc(uint32_t value, volatile void *addr)
{
    rte_write32(value, addr);
}

static __rte_always_inline void
rte_write64(uint64_t value, volatile void *addr)
{
	rte_io_wmb();
	rte_write64_relaxed(value, addr);
}

#ifdef __cplusplus
}
#endif

#endif /* _RTE_IO_S390X_H_ */
