/* SPDX-License-Identifier: BSD-3-Clause
 * (c) Copyright IBM Corp. 2018, 2019
 */

#include "rte_cpuflags.h"

#include <elf.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

/* Symbolic values for the entries in the auxiliary table */
#define AT_HWCAP  16
#define AT_HWCAP2 26

/* software based registers */
enum cpu_register_t {
	REG_NONE = 0,
	REG_HWCAP,
	REG_HWCAP2,
	REG_MAX
};

typedef uint32_t hwcap_registers_t[REG_MAX];

struct feature_entry {
	uint32_t reg;
	uint32_t bit;
#define CPU_FLAG_NAME_MAX_LEN 64
	char name[CPU_FLAG_NAME_MAX_LEN];
};

#define FEAT_DEF(name, reg, bit) \
	[RTE_CPUFLAG_##name] = {reg, bit, #name},

const struct feature_entry rte_cpu_feature_table[] = {
	FEAT_DEF(ESAN3,                  REG_HWCAP,   0)
	FEAT_DEF(ZARCH,                  REG_HWCAP,   1)
	FEAT_DEF(STFLE,                  REG_HWCAP,   2)
	FEAT_DEF(MSA,                    REG_HWCAP,   3)
	FEAT_DEF(LDISP,                  REG_HWCAP,   4)
	FEAT_DEF(EIMM,                   REG_HWCAP,   5)
	FEAT_DEF(DFP,                    REG_HWCAP,   6)
	FEAT_DEF(HPAGE,                  REG_HWCAP,   7)
	FEAT_DEF(ETF3EH,                 REG_HWCAP,   8)
	FEAT_DEF(HIGH_GPRS,              REG_HWCAP,   9)
	FEAT_DEF(TE,                     REG_HWCAP,  10)
	FEAT_DEF(VXRS,                   REG_HWCAP,  11)
	FEAT_DEF(VXRS_BCD,               REG_HWCAP,  12)
	FEAT_DEF(VXRS_EXT,               REG_HWCAP,  13)
	FEAT_DEF(GS,                     REG_HWCAP,  14)
};

/*
 * Read AUXV software register and get cpu features for Power
 */
static void
rte_cpu_get_features(hwcap_registers_t out)
{
	out[REG_HWCAP] = rte_cpu_getauxval(AT_HWCAP);
	out[REG_HWCAP2] = rte_cpu_getauxval(AT_HWCAP2);
}

/*
 * Checks if a particular flag is available on current machine.
 */
int
rte_cpu_get_flag_enabled(enum rte_cpu_flag_t feature)
{
	const struct feature_entry *feat;
	hwcap_registers_t regs = {0};

	if (feature >= RTE_CPUFLAG_NUMFLAGS)
		return -ENOENT;

	feat = &rte_cpu_feature_table[feature];
	if (feat->reg == REG_NONE)
		return -EFAULT;

	rte_cpu_get_features(regs);
	return (regs[feat->reg] >> feat->bit) & 1;
}

const char *
rte_cpu_get_flag_name(enum rte_cpu_flag_t feature)
{
	if (feature >= RTE_CPUFLAG_NUMFLAGS)
		return NULL;
	return rte_cpu_feature_table[feature].name;
}
