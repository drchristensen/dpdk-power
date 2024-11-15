/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2023 Napatech A/S
 */

#include <stdint.h>
#include <stdlib.h>

#include "flow_api_engine.h"

#define OWNER_ID_COUNT 256
#define PORT_COUNT 8

struct group_lookup_entry_s {
	uint64_t ref_counter;
	uint32_t *reverse_lookup;
};

struct group_handle_s {
	uint32_t group_count;

	uint32_t *translation_table;

	struct group_lookup_entry_s *lookup_entries;
};

int flow_group_handle_create(void **handle, uint32_t group_count)
{
	struct group_handle_s *group_handle;

	*handle = calloc(1, sizeof(struct group_handle_s));
	group_handle = *handle;

	group_handle->group_count = group_count;
	group_handle->translation_table =
		calloc((uint32_t)(group_count * PORT_COUNT * OWNER_ID_COUNT), sizeof(uint32_t));
	group_handle->lookup_entries = calloc(group_count, sizeof(struct group_lookup_entry_s));

	return *handle != NULL ? 0 : -1;
}

int flow_group_handle_destroy(void **handle)
{
	if (*handle) {
		struct group_handle_s *group_handle = (struct group_handle_s *)*handle;

		free(group_handle->translation_table);
		free(group_handle->lookup_entries);

		free(*handle);
		*handle = NULL;
	}

	return 0;
}

int flow_group_translate_get(void *handle, uint8_t owner_id, uint8_t port_id, uint32_t group_in,
	uint32_t *group_out)
{
	struct group_handle_s *group_handle = (struct group_handle_s *)handle;
	uint32_t *table_ptr;
	uint32_t lookup;

	if (group_handle == NULL || group_in >= group_handle->group_count || port_id >= PORT_COUNT)
		return -1;

	/* Don't translate group 0 */
	if (group_in == 0) {
		*group_out = 0;
		return 0;
	}

	table_ptr = &group_handle->translation_table[port_id * OWNER_ID_COUNT * PORT_COUNT +
		owner_id * OWNER_ID_COUNT + group_in];
	lookup = *table_ptr;

	if (lookup == 0) {
		for (lookup = 1; lookup < group_handle->group_count &&
			group_handle->lookup_entries[lookup].ref_counter > 0;
			++lookup)
			;

		if (lookup < group_handle->group_count) {
			group_handle->lookup_entries[lookup].reverse_lookup = table_ptr;
			group_handle->lookup_entries[lookup].ref_counter += 1;

			*table_ptr = lookup;

		} else {
			return -1;
		}

	} else {
		group_handle->lookup_entries[lookup].ref_counter += 1;
	}

	*group_out = lookup;
	return 0;
}
