#include "socklib.h"
#include "log.h"
#include "util.h"
#include "participants.h"
#include "udpcast.h"
#ifdef USE_SYSLOG
#include <syslog.h>
#endif

struct  participantsDb {
    uint32_t nrParticipants;
    
    struct clientDesc {
	struct sockaddr_in addr;
	int used;
	uint32_t capabilities;
	unsigned int rcvbuf;
    } clientTable[MAX_CLIENTS];
};

uint32_t addParticipant(participantsDb_t,
			struct sockaddr_in *addr, 
			uint32_t capabilities, 
			unsigned int rcvbuf,
			int pointopoint);

int isParticipantValid(struct participantsDb *db, uint32_t i) {
    return db->clientTable[i].used;
}

int removeParticipant(struct participantsDb *db, uint32_t i) {
    if(db->clientTable[i].used) {
	char ipBuffer[16];	
	flprintf("Disconnecting #%d (%s)\n", i, 
		 getIpString(&db->clientTable[i].addr, ipBuffer));
#ifdef USE_SYSLOG
	syslog(LOG_INFO, "Disconnecting #%d (%s)\n", i,
			getIpString(&db->clientTable[i].addr, ipBuffer));
#endif
	db->clientTable[i].used = 0;
	db->nrParticipants--;
    }
    return 0;
}

uint32_t lookupParticipant(struct participantsDb *db,
			   struct sockaddr_in *addr) {
    unsigned int i;
    for (i=0; i < MAX_CLIENTS; i++) {
	if (db->clientTable[i].used && 
	    ipIsEqual(&db->clientTable[i].addr, addr)) {
	    return i;
	}
    }
    return UINT32_MAX;
}

uint32_t nrParticipants(participantsDb_t db) {
    return db->nrParticipants;
}

uint32_t addParticipant(participantsDb_t db,
			struct sockaddr_in *addr, 
			uint32_t capabilities,
			unsigned int rcvbuf,
			int pointopoint) {
    unsigned int i;

    if((i = lookupParticipant(db, addr)) != UINT32_MAX)
	return i;

    for (i=0; i < MAX_CLIENTS; i++) {
	if (!db->clientTable[i].used) {
	    char ipBuffer[16];
	    db->clientTable[i].addr = *addr;
	    db->clientTable[i].used = 1;
	    db->clientTable[i].capabilities = capabilities;
	    db->clientTable[i].rcvbuf = rcvbuf;
	    db->nrParticipants++;

	    fprintf(stderr, "New connection from %s  (#%d) %08x\n", 
		    getIpString(addr, ipBuffer), i, capabilities);
#ifdef USE_SYSLOG
	    syslog(LOG_INFO, "New connection from %s  (#%d)\n",
			    getIpString(addr, ipBuffer), i);
#endif
	    return i;
	} else if(pointopoint)
	    return UINT32_MAX;
    }

    return UINT32_MAX; /* no space left in participant's table */
}

participantsDb_t makeParticipantsDb(void)
{
    return MALLOC(struct participantsDb);
}

uint32_t getParticipantCapabilities(participantsDb_t db, uint32_t i)
{
    return db->clientTable[i].capabilities;
}

unsigned int getParticipantRcvBuf(participantsDb_t db, uint32_t i)
{
    return db->clientTable[i].rcvbuf;
}

struct sockaddr_in *getParticipantIp(participantsDb_t db,
				     uint32_t i)
{
    return &db->clientTable[i].addr;
}
    
void printNotSet(participantsDb_t db, char *d)
{
    int first=1;
    unsigned int i;
    flprintf("[");
    for (i=0; i < MAX_CLIENTS; i++) {
	if (db->clientTable[i].used) {
	    if(!BIT_ISSET(i, d)) {
		if(!first)
		    flprintf(",");
		first=0;
		flprintf("%d", i);
	    }
	}
    }
    flprintf("]");
}


void printSet(participantsDb_t db, char *d)
{
    int first=1;
    unsigned int i;
    flprintf("[");
    for (i=0; i < MAX_CLIENTS; i++) {
	if (db->clientTable[i].used) {
	    if(BIT_ISSET(i, d)) {
		if(!first)
		    flprintf(",");
		first=0;
		flprintf("%d", i);
	    }
	}
    }
    flprintf("]");
}
