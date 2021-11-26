/* libbb_udhcp.h - busybox compatability wrapper */

#ifndef _LIBBB_UDPCAST_H
#define _LIBBB_UDPCAST_H

#ifndef NO_BB
#include "libbb.h"
#include "busybox.h"

#define COMBINED_BINARY

#else /* ! BB_BT */

#define TRUE			1
#define FALSE			0

#include <malloc.h>
#define xmalloc malloc

#endif /* BB_VER */

#endif /* _LIBBB_UDPCAST_H */
