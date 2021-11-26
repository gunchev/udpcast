#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include "log.h"
#include "udpcast.h"
#include "udp-receiver.h"
#include "socklib.h"
#include "udpc_version.h"

#ifdef USE_SYSLOG
#include <syslog.h>
#endif

#ifdef BB_FEATURE_UDPCAST_FEC
#include "fec.h"
#endif

#define MAXBLOCK 1024

static struct option options[] = {
    { "file", 1, NULL, 'f' },
    { "pipe", 1, NULL, 'p' },
    { "port", 1, NULL, 'P' },
    { "portbase", 1, NULL, 'P' },
    { "interface", 1, NULL, 'i' },
    { "ttl", 1, NULL, 0x0401 },
    { "mcast_all_address", 1, NULL, 'M' }, /* Obsolete name */
    { "mcast-all-address", 1, NULL, 'M' }, /* Obsolete name */

    { "mcast_rdv_address", 1, NULL, 'M' },
    { "mcast-rdv-address", 1, NULL, 'M' },


#ifdef LOSSTEST
    /* simulating packet loss */
    { "write-loss", 1, NULL, 0x601 },
    { "read-loss", 1, NULL, 0x602 },
    { "seed", 1, NULL, 0x603 },
    { "print-seed", 0, NULL, 0x604 },
    { "read-swap", 1, NULL, 0x605 },
#endif

    { "passive", 0, NULL, 0x0801 },

    { "nosync", 0, NULL, 0x0901 },

    { "rcvbuf", 1, NULL, 0x0a01 },

    { "nokbd", 0, NULL, 0xb04 },

    { "exitWait", 1, NULL, 0xd01 }, /* Obsolete name */
    { "exit-wait", 1, NULL, 0xd01 },

#ifdef BB_FEATURE_UDPCAST_FEC
    { "license", 0, NULL, 'L' },
#endif

    { "log", 1, NULL, 'l' },
};

static void intHandler(int nr) {
    udpc_fatal(1, "Cancelled by user\n");
}

#ifdef NO_BB
static void usage(char *progname) {
    fprintf(stderr, "%s [--file file] [--pipe pipe] [--portbase portbase] [--interface net-interface] [--log file] [--ttl time-to-live] [--mcast-rdv-addr mcast-rdv-address] [--rcvbuf buf] [--nokbd] [--exit-wait milliseconds] [--nosync] [--license]\n", 
	    progname);
    exit(1);
}
#endif

#ifndef NO_BB
int udpreceiver_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
    char *ptr;
    struct net_config net_config;
    struct disk_config disk_config;
    int c;
    int doWarn=0;
    char *ifName=NULL;

#ifdef LOSSTEST
    int seedSet = 0;
    int printSeed = 0;
#endif

    disk_config.fileName=NULL;
    disk_config.pipeName=NULL;
    disk_config.flags = 0;

    net_config.portBase = 9000;
    net_config.rateLimit = NULL;
    net_config.ttl = 1;
    net_config.flags = 0;
    net_config.mcastRdv = NULL;
    net_config.exitWait = 500;

#ifdef WINDOWS
    /* windows is basically unusable with its default buffer size of 8k...*/
    net_config.requestedBufSize = 1024*1024;
#else
    net_config.requestedBufSize = 0;
#endif

    ptr = strrchr(argv[0], '/');
    if(!ptr)
	ptr = argv[0];
    else
	ptr++;
    
    net_config.net_if = NULL;
    if (strcmp(ptr, "init") == 0) {
	doWarn = 1;
	disk_config.pipeName = strdup("/bin/gzip -dc");
	disk_config.fileName = "/dev/hda";
    }
    while( (c=getopt_long(argc, argv, "f:p:P:i:l:M:L",
			  options, NULL)) != EOF ) {
	switch(c) {
	    case 'f':
		disk_config.fileName=optarg;
		break;
	    case 'i':
		ifName=optarg;
		break;
	    case 'p':
		disk_config.pipeName=optarg;
		break;
	    case 'P':
		net_config.portBase = atoi(optarg);
		break;
	    case 'l':
		udpc_log = fopen(optarg, "a");
		break;
	    case 0x0401:
		net_config.ttl = atoi(optarg);
		break;
	    case 'M':
		net_config.mcastRdv = strdup(optarg);
		break;

#ifdef BB_FEATURE_UDPCAST_FEC
	    case 'L':
		    fec_license();
		    break;
#endif

#ifdef LOSSTEST
	    case 0x601:
		setWriteLoss(optarg);
		break;
	    case 0x602:
		setReadLoss(optarg);
		break;
	    case 0x603:
		seedSet=1;
		srandom(strtoul(optarg,0,0));
		break;
	    case 0x604:
		printSeed=1;
		break;
	    case 0x605:
		setReadSwap(optarg);
		break;
#endif
	    case 0x801:
		net_config.flags|=FLAG_PASSIVE;
		break;
	    case 0x901:
		disk_config.flags|=FLAG_NOSYNC;
		break;
	    case 0xa01:
		net_config.requestedBufSize=parseSize(optarg);
		break;
	    case 0xb04:
		net_config.flags |= FLAG_NOKBD;
		break;

	    case 0xd01:
		net_config.exitWait = atoi(optarg);
		break;

	    case '?':
#ifndef NO_BB
	        bb_show_usage();
#else
		usage(argv[0]);
#endif
	}
    }

    fprintf(stderr, "Udp-receiver %s\n", version);

#ifdef LOSSTEST
    if(!seedSet)
	srandomTime(printSeed);
#endif

    signal(SIGINT, intHandler);
#ifdef USE_SYSLOG
    openlog((const char *)"udpcast", LOG_NDELAY|LOG_PID, LOG_SYSLOG);
#endif
    
    return startReceiver(doWarn, &disk_config, &net_config, ifName);
}
