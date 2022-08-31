#ifndef UDPCAST_CONFIG_H
# define UDPCAST_CONFIG_H
# include "config.h"
#endif

#include <assert.h>
#include <errno.h>
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include "socklib.h"
#include "rate-limit.h"
#include "util.h"
#include "log.h"
#include "rateGovernor.h"

struct rate_limit {
    uint64_t date;
    uint64_t realDate;
    uint64_t bitrate;
    uint64_t queueSize;
};

#define MILLION 1000000U

static uint64_t parseSpeed(const char *speedString) {
    char *eptr;
    uint64_t speed;
    assert(speedString);
    if (*speedString == '\0')
	fatal(1, "Empty string specified for speed!\n");
    errno = 0;
#ifdef HAVE_STRTOULL
    speed = strtoul(speedString, &eptr, 10);
    if(speed > ULONG_MAX / 2)
	 fatal(1, "Speed out of range!\n"); 
#else
    speed = strtoull(speedString, &eptr, 10);
    if(speed > UINT64_MAX / 2)
	 fatal(1, "Speed out of range!\n"); 
#endif
    if (errno == ERANGE)
	 fatal(1, "Speed out of range!\n"); 
    if(eptr && *eptr) {
	switch(*eptr) {
	    case 'g':
	    case 'G':
	        if (speed > UINT64_MAX / 1000000000)
		    fatal(1, "Speed out of range!\n");
		speed *= 1000000000;
		break;
	    case 'm':
	    case 'M':
		if (speed > UINT64_MAX / 1000000)
                    fatal(1, "Speed out of range!\n");  
		speed *= 1000000;
		break;
	    case 'k':
	    case 'K':
		if (speed > UINT64_MAX / 1000)
                    fatal(1, "Speed out of range!\n"); 
		speed *= 1000;
		break;
	    case '\0':
		break;
	    default:
		udpc_fatal(1, "Unit %c unsupported\n", *eptr);
	}
	++eptr;
        if (*eptr != '\0') {
            udpc_fatal(1, "Valid speed input ends with garbage '%s'!\n", eptr);
        }
    }
    if (speed < 1)
        udpc_fatal(1, "Speed limit of 0 bits per second takes forever!\n");
    return speed;
}

static uint64_t getU64Date(void) {
    uint64_t date;
    struct timeval tv;
    gettimeofday(&tv,0);
    date = (uint64_t) tv.tv_sec;
    date *= MILLION;
    date += (uint64_t) tv.tv_usec;
    return date;
}

static void *allocRateLimit(void) {
    struct rate_limit *rateLimit = MALLOC(struct rate_limit);
    if(rateLimit == NULL)
	return NULL;
    rateLimit->date = getU64Date();
    rateLimit->bitrate = 0;
    rateLimit->queueSize = 0;
    return rateLimit;
}

static void setProp(void *data, const char *key, const char *bitrate) {
    struct rate_limit *rateLimit = (struct rate_limit *) data;
    if(rateLimit == NULL)
	    return;
    if(!strcmp(MAX_BITRATE, key))
	rateLimit->bitrate = parseSpeed(bitrate);
}

static void doRateLimit(void *data, int fd, in_addr_t ip, unsigned long size) {
    struct rate_limit *rateLimit = (struct rate_limit *) data;
    (void) fd;
    (void) ip;
    if(rateLimit) {
	uint64_t now = getU64Date();
	uint64_t elapsed = now - rateLimit->date;
	uint64_t bits = elapsed * ((uint64_t)rateLimit->bitrate) / MILLION;
	uint64_t sleepTime;
	size += 28; /* IP header size */

	if(bits >= rateLimit->queueSize * 8) {
	    rateLimit->queueSize = size;
	    rateLimit->date = now;
	    return;
	}
	
	rateLimit->queueSize -= bits / 8;
	rateLimit->date += bits * MILLION / rateLimit->bitrate;
	rateLimit->realDate = now;
	sleepTime = rateLimit->queueSize * 8 * MILLION / rateLimit->bitrate;
	if(sleepTime > 40000 || rateLimit->queueSize >= 100000) {
	    sleepTime -= 10000;
	    sleepTime -= sleepTime % 10000;
	    usleep((useconds_t)sleepTime);
	}
	rateLimit->queueSize += size;
    }
}

rateGovernor_t maxBitrate = {
    allocRateLimit,
    setProp,
    NULL,
    doRateLimit,
    NULL
};
