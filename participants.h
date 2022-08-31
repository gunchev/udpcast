#ifndef PARTICIPANTS_H
#define PARTICIPANTS_H

#define MAX_CLIENTS 1024

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

typedef struct participantsDb *participantsDb_t;

#define isParticipantValid udpc_isParticipantValid
#define removeParticipant udpc_removeParticipant
#define lookupParticipant udpc_lookupParticipant
#define nrParticipants udpc_nrParticipants
#define addParticipant udpc_addParticipant
#define makeParticipantsDb udpc_makeParticipantsDb
#define getParticipantCapabilities udpc_getParticipantCapabilities
#define getParticipantRcvBuf udpc_getParticipantRcvBuf
#define getParticipantIp udpc_getParticipantIp
#define printNotSet udpc_printNotSet
#define printSet udpc_printSet

int udpc_isParticipantValid(participantsDb_t, uint32_t slot);
int udpc_removeParticipant(participantsDb_t, uint32_t slot);
uint32_t udpc_lookupParticipant(participantsDb_t, struct sockaddr_in *addr);
uint32_t udpc_nrParticipants(participantsDb_t);
uint32_t udpc_addParticipant(participantsDb_t,
			     struct sockaddr_in *addr, 
			     uint32_t capabilities, 
			     unsigned int rcvbuf,
			int pointopoint);
participantsDb_t udpc_makeParticipantsDb(void);
uint32_t udpc_getParticipantCapabilities(participantsDb_t db, uint32_t i);
unsigned int udpc_getParticipantRcvBuf(participantsDb_t db, uint32_t i);
struct sockaddr_in *udpc_getParticipantIp(participantsDb_t db, uint32_t i);
void udpc_printNotSet(participantsDb_t db, char *d);
void udpc_printSet(participantsDb_t db, char *d);

#endif
