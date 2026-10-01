#ifndef ARX_FORCE_GNU_H
#define ARX_FORCE_GNU_H
#undef _POSIX_C_SOURCE
#undef _POSIX_SOURCE
#undef _DEFAULT_SOURCE
#define _GNU_SOURCE 1
#include <time.h>
#include <unistd.h>
#endif
