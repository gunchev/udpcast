
#include "config.h"
#include "strparse.h"
#include <stdlib.h>
#include <errno.h>

static long strtol_with_range(const char *nptr, char **endptr, int base,
			      long min, long max) {
	long l;
	errno = 0;
	l = strtol(nptr, endptr, base);
	if(errno != 0)
		return l;
	if(l > max) {
		errno = ERANGE;
		return max;
	}
	if(l < min) {
		errno = ERANGE;
		return min;
	}
	return l;
}

unsigned short strtous(const char *nptr, char **endptr, int base) {
	return (unsigned short) strtol_with_range(nptr, endptr, base,
						  0, USHRT_MAX);
}

#if UINT_MAX != ULONG_MAX
unsigned int strtoui(const char *nptr, char **endptr, int base) {
	return (unsigned int) strtol_with_range(nptr, endptr, base,
						0, UINT_MAX);
}
int strtoi(const char *nptr, char **endptr, int base) {
	return (int) strtol_with_range(nptr, endptr, base,
				       INT_MIN, INT_MAX);
}
#endif
