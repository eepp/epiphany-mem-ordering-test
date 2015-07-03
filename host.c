#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <e-hal.h>

#include "common.h"

static int init_smem(e_mem_t *smem)
{
	int ret = 0;

	e_set_host_verbosity(H_D0);
	printf("Initializing HAL\n");

	if (e_init(NULL) != E_OK) {
		fprintf(stderr, "Error: Epiphany HAL initialization failed\n");
		ret = -1;
		goto error;
	}

	printf("HAL initialized\n");
	printf("Allocating %u bytes of shared memory in region \"%s\"\n",
		sizeof(struct shared_stuff), SHM_NAME);
	ret = e_shm_alloc(smem, SHM_NAME, sizeof(struct shared_stuff)) != E_OK;

	if (ret != E_OK) {
		fprintf(stderr, "Warning: failed to allocate shared memory region: %s\n",
			strerror(errno));
		printf("Attaching to shared memory region \"%s\"\n", SHM_NAME);
		ret = e_shm_attach(smem, SHM_NAME);
	}

	if (ret != E_OK) {
		fprintf(stderr, "Error: failed to attach to shared memory: %s\n",
			strerror(errno));
		ret = -1;
		goto error_finalize;
	}

	memset(smem->base, 0, sizeof(struct shared_stuff));

	return 0;

error_finalize:
	if (smem->base) {
		e_shm_release(SHM_NAME);
	}

	e_finalize();

error:
	return ret;
}

static volatile int quit = 0;

static void sig_handler(int signo)
{
	if (signo == SIGINT) {
		quit = 1;
		fprintf(stderr, "\nGot SIGINT: quitting\n");
	}
}

int main(int argc, char *argv[])
{
	e_mem_t smem;
	volatile struct shared_stuff *shared_stuff;

	signal(SIGINT, sig_handler);

	if (init_smem(&smem)) {
		fprintf(stderr, "Error: init_smem() failed\n");
		return 1;
	}

	shared_stuff = (volatile struct shared_stuff *) smem.base;

	for (;;) {
		int i;

		for (i = 0; i < 16; ++i) {
			volatile struct per_core_stuff *per_core_stuff =
				(volatile struct per_core_stuff *) &shared_stuff->stuff[i];

			if (per_core_stuff->err_flag) {
				printf("Core %u: read=%u expected=%u\n",
					i, per_core_stuff->err_read,
					per_core_stuff->err_expected);
			}
		}

		if (quit) {
			break;
		}

		sleep(1);
	}

	if (smem.base) {
		e_shm_release(SHM_NAME);
	}

	e_finalize();

	return 0;
}
