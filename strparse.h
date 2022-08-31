#ifndef UDPCAST_CONFIG_H
# define UDPCAST_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif 

#include <limits.h>

unsigned short strtous(const char *nptr, char **endptr, int base);


#if UINT_MAX != ULONG_MAX
unsigned int strtoui(const char *nptr, char **endptr, int base);
int strtoi(const char *nptr, char **endptr, int base);
#else
#define strtoui strtoul
#define strtoi strtol
#endif
