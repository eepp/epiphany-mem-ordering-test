#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <e_lib.h>

#include "common.h"

int main(void)
{
	e_memseg_t smem;
	e_coreid_t coreid;
	uint32_t local_read;
	uint32_t local_expected;
	unsigned int row, col, index;
	volatile struct shared_stuff *shared_stuff;
	volatile struct per_core_stuff *per_core_stuff;

	/* attach to shared memory */
	if (e_shm_attach(&smem, SHM_NAME) != E_OK) {
		return 1;
	}

	coreid = e_get_coreid();
	e_coords_from_coreid(coreid, &row, &col);
	index = row + col * 4;
	shared_stuff = (volatile struct shared_stuff *) smem.ephy_base;
	per_core_stuff =
		(volatile struct per_core_stuff *) &shared_stuff->stuff[index];

	for (;;) {
		int nop;

		local_expected = per_core_stuff->val + 1;
		per_core_stuff->val = local_expected;

		for (nop = 0; nop < NOP_COUNT; ++nop) {
			__asm__ __volatile__("nop");
		}

		local_read = per_core_stuff->val;

		if (local_read != local_expected) {
			per_core_stuff->err_expected = local_expected;
			per_core_stuff->err_read = local_read;
			per_core_stuff->err_flag = 1;
			break;
		}
	}

	return 0;
}
