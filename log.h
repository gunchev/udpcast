#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include "libbb_udpcast.h"
#include "udpcast.h"

#define logprintf udpc_logprintf
#define flprintf udpc_flprintf
#define fatal udpc_fatal
#define printLongNum udpc_printLongNum 

/*void printNewlineIfNeeded(void);*/
int logprintf(FILE *logfile, const char *fmt, ...)
    ATTRIBUTE ((format (printf, 2, 3)));
int flprintf(const char *fmt, ...) 
    ATTRIBUTE ((format (printf, 1, 2)));
int fatal(int code, const char *fmt, ...) 
    ATTRIBUTE ((noreturn)) ATTRIBUTE ((format (printf, 2, 3)));

int printLongNum(unsigned long long x);

extern FILE *udpc_log;

#endif
