
#ifndef __libfmemopen_windows
#define __libfmemopen_windows

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

FILE *fmemopen(void *buf, size_t len, const char *mode);

#ifdef __cplusplus
}
#endif

#endif
