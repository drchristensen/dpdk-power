/* SPDX-License-Identifier: BSD-3-Clause
 * (c) Copyright IBM Corp. 2018, 2019
 */

#include "rte_hypervisor.h"

enum rte_hypervisor
rte_hypervisor_get(void)
{
	return RTE_HYPERVISOR_UNKNOWN;
}
