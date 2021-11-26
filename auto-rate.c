#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

#include "socklib.h"
#include "udp-sender.h"
#include "auto-rate.h"
#include "util.h"
#include "rateGovernor.h"

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef FLAG_AUTORATE

struct auto_rate_t {
  int isInitialized; /* has this already been initialized? */
  int dir; /* 1 if TIOCOUTQ is remaining space, 
	    * 0 if TIOCOUTQ is consumed space */
  int sendbuf; /* sendbuf */
};

static int getCurrentQueueLength(int sock) {
#ifdef TIOCOUTQ
    int length;
    if(ioctl(sock, TIOCOUTQ, &length) < 0)
	return -1;
    return length;
#else
    return -1;
#endif
}

static void *allocAutoRate(void) {
    struct auto_rate_t *autoRate = MALLOC(struct auto_rate_t);
    if(autoRate == NULL)
	    return NULL;
    autoRate->isInitialized = 0;
    return autoRate;
}

static void initialize(struct auto_rate_t *autoRate, int sock) {
  int q = getCurrentQueueLength(sock);
  if(q == 0) {
    autoRate->dir = 0;
    autoRate->sendbuf = getSendBuf(sock);
  } else {
    autoRate->dir = 1;
    autoRate->sendbuf = q;
  }
  autoRate->isInitialized=1;
}

/**
 * If queue gets almost full, slow down things
 */
static void doAutoRate(void *data, int sock, in_addr_t ip, long size)
{
    struct auto_rate_t *autoRate = (struct auto_rate_t*) data;
    (void) ip;

    if(!autoRate->isInitialized)
      initialize(autoRate, sock);

    while(1) {
	int r = getCurrentQueueLength(sock);
	if(autoRate->dir)
	    r = autoRate->sendbuf - r;

	if(r < autoRate->sendbuf / 2 - size)
	    return;
#if DEBUG
	flprintf("Queue full %d/%d... Waiting\n", r, autoRate->sendbuf);
#endif
	usleep(2500);
    }
}

rateGovernor_t autoRate = {
    allocAutoRate,
    NULL,
    NULL,
    doAutoRate,
    NULL
};

#endif
