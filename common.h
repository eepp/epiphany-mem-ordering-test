#ifndef _COMMON_H
#define _COMMOH_H

#include <stdint.h>

#define SHM_NAME	"mem-ordering-test"

struct per_core_stuff {
	uint32_t val;
	uint32_t err_expected, err_read;
	int err_flag;
};

struct shared_stuff {
	struct per_core_stuff stuff[16];
};

#endif /* _COMMOH_H */
