#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include "socklib.h"
#include "rate-limit.h"
#include "util.h"
#include "log.h"
#include "rateGovernor.h"

struct rate_limit {
    long long date;
    long long realDate;
    unsigned long bitrate;
    unsigned long queueSize;
};

#define LLMILLION 1000000ULL

static unsigned long parseSpeed(const char *speedString) {
    char *eptr;
    unsigned long speed;
    char c;
    assert(speedString);
    if (*speedString == '\0')
	fatal(1, "Empty string specified for speed!\n");
    c = (*speedString < ' ' || *speedString > '~') ? '@' : *speedString; /* don't print non-ASCII characters */
    if (c < '0' || c > '9') /* good enough to stop negative values */
	fatal(1, "Speed starts with invalid character '%c'!\n", c);
    speed = strtoul(speedString, &eptr, 10);
    if (speed == ULONG_MAX && errno == ERANGE)
	fatal(1, "Speed out of range!\n");
    if(eptr && *eptr) {
	switch(*eptr) {
	    case 'g':
	    case 'G':
		if (speed > ULONG_MAX / 1000000000ull)
		    fatal(1, "Speed out of range!\n");
		speed *= 1000000000ull;
		break;
	    case 'm':
	    case 'M':
		if (speed > ULONG_MAX / 1000000ull)
		    fatal(1, "Speed out of range!\n");
		speed *= 1000000ull;
		break;
	    case 'k':
	    case 'K':
		if (speed > ULONG_MAX / 1000ull)
		    fatal(1, "Speed out of range!\n");
		speed *= 1000ull;
		break;
	    case '\0':
		break;
	    default:
		c = (*eptr < ' ' || *eptr > '~') ? '@' : *eptr; /* don't print non-ASCII characters */
		udpc_fatal(1, "Unit '%c' unsupported\n", c);
	}
	++eptr;
	if (*eptr != '\0') {
	    c = (*eptr < ' ' || *eptr > '~') ? '@' : *eptr; /* don't print non-ASCII characters */
	    udpc_fatal(1, "Valid speed input ends with garbage '%c'!\n", c);
	}
    }
    if (speed < 1)
	udpc_fatal(1, "Speed limit of 0 bits per second takes forever!\n");
    return speed;
}

static long long getLongLongDate(void) {
    long long date;
    struct timeval tv;
    gettimeofday(&tv,0);
    date = (long long) tv.tv_sec;
    date *= LLMILLION;
    date += (long long) tv.tv_usec;
    return date;
}

static void *allocRateLimit(void) {
    struct rate_limit *rateLimit = MALLOC(struct rate_limit);
    if(rateLimit == NULL)
	return NULL;
    rateLimit->date = getLongLongDate();
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

static void doRateLimit(void *data, int fd, in_addr_t ip, long size) {
    struct rate_limit *rateLimit = (struct rate_limit *) data;
    (void) fd;
    (void) ip;
    if(rateLimit) {
	long long now = getLongLongDate();
	long long elapsed = now - rateLimit->date;
	unsigned long long bits = elapsed * ((unsigned long long)rateLimit->bitrate) / LLMILLION;
	int sleepTime;
	size += 28; /* IP header size */

	if(bits >= rateLimit->queueSize * 8) {
	    rateLimit->queueSize = size;
	    rateLimit->date = now;
	    return;
	}

	rateLimit->queueSize -= bits / 8;
	rateLimit->date += bits * LLMILLION / rateLimit->bitrate;
	rateLimit->realDate = now;
	sleepTime = rateLimit->queueSize * 8 * LLMILLION / rateLimit->bitrate;
	if(sleepTime > 40000 || rateLimit->queueSize >= 100000) {
	    sleepTime -= 10000;
	    sleepTime -= sleepTime % 10000;
	    usleep(sleepTime);
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
