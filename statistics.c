#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#include <sys/time.h>
#include <unistd.h>
#include "log.h"
#include "statistics.h"
#include "util.h"
#include "socklib.h"

struct receiver_stats {
    struct timeval tv_start;
    int bytesOrig;
    long long totalBytes;
    int timerStarted;
    int fd;
};

receiver_stats_t allocReadStats(int fd) {
    receiver_stats_t rs =  MALLOC(struct receiver_stats);
    rs->fd = fd;
    return rs;
}

void receiverStatsAddBytes(receiver_stats_t rs, long bytes) {
    if(rs != NULL)
	rs->totalBytes += bytes;
}

void receiverStatsStartTimer(receiver_stats_t rs) {
    if(rs != NULL && !rs->timerStarted) {
	gettimeofday(&rs->tv_start, 0);
	rs->timerStarted = 1;
    }
}

static void printFilePosition(int fd) {
#ifndef WINDOWS
    if(fd != -1) {
	loff_t offset = lseek64(fd, 0, SEEK_CUR);
	printLongNum(offset);
    }
#endif
}

void displayReceiverStats(receiver_stats_t rs) {
    long long timePassed;
    struct timeval tv_now;

    if(rs == NULL)
	return;

    gettimeofday(&tv_now, 0);

    fprintf(stderr, "bytes=");
    printLongNum(rs->totalBytes);
    fprintf(stderr, " (");
    
    timePassed = tv_now.tv_sec - rs->tv_start.tv_sec;
    timePassed *= 1000000;
    timePassed += tv_now.tv_usec - rs->tv_start.tv_usec;
    if (timePassed != 0) {
	int mbps = (int) (rs->totalBytes * 800 / timePassed);
	fprintf(stderr, "%3d.%02d", mbps / 100, mbps % 100);
    } else {
	fprintf(stderr, "***.**");
    }
    fprintf(stderr, " Mbps)");
    printFilePosition(rs->fd);
    fprintf(stderr, "\r");
    fflush(stderr);
}


struct sender_stats {
    unsigned long long totalBytes;
    unsigned long long retransmissions;
    int fd;
    int clNo;
};

sender_stats_t allocSenderStats(int fd) {
    sender_stats_t ss = MALLOC(struct sender_stats);
    ss->fd = fd;
    return ss;
}

void senderStatsAddBytes(sender_stats_t ss, long bytes) {
    if(ss != NULL)
	ss->totalBytes += bytes;
}

void senderStatsAddRetransmissions(sender_stats_t ss, int retransmissions) {
    if(ss != NULL)
	ss->retransmissions += retransmissions;
}


void displaySenderStats(sender_stats_t ss, int blockSize, int sliceSize) {
    unsigned int blocks, percent;
    
    if(ss == NULL)
	return;
    blocks = (ss->totalBytes + blockSize - 1) / blockSize;
    percent = (1000L * ss->retransmissions) / blocks;
    
    fprintf(stderr, "bytes=");
    printLongNum(ss->totalBytes);
    fprintf(stderr, "re-xmits=%07llu (%3u.%01u%%) slice=%04d ",
	    ss->retransmissions, percent / 10, percent % 10, sliceSize);
    printFilePosition(ss->fd);
    fprintf(stderr, "- %3d\r", ss->clNo);
    fflush(stderr);
}

void senderSetAnswered(sender_stats_t ss, int clNo) {
    if(ss != NULL)
	ss->clNo = clNo;
}
