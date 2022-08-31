#ifndef UDPC_PROTOC_H
#define UDPC_PROTOC_H

#include "udpcast.h"

#define MAX_BLOCK_SIZE 1456
#define MAX_FEC_INTERLEAVE 256

/**
 * This file describes the UDPCast protocol
 */
enum opCode {    
    /* Receiver to sender */

    CMD_OK,	     /* all is ok, no need to retransmit anything */
    CMD_RETRANSMIT,  /* receiver asks for some data to be retransmitted */
    CMD_GO,	     /* receiver tells server to start */
    CMD_CONNECT_REQ, /* receiver tries to find out server's address */
    CMD_DISCONNECT,  /* receiver wants to disconnect itself */

    CMD_UNUSED,	     /* obsolete version of CMD_HELLO, dating back to the
		      * time when we had little endianness (PC). This
		      * opcode contained a long unnoticed bug with parsing of
		      * blocksize */

    /* Sender to receiver */
    CMD_REQACK,	     /* server request acknowledgments from receiver */
    CMD_CONNECT_REPLY, /* receiver tries to find out server's address */

    CMD_DATA,        /* a block of data */
    CMD_FEC,	     /* a forward-error-correction block */

    CMD_HELLO_NEW,	  /* sender says he's up */
    CMD_HELLO_STREAMING,  /* retransmitted hello during streaming mode */
};

/* Sender says he's up. This is not in the enum with the others,
 * because of some endianness Snafu in early versions. However,since
 * 2005-12-23, new receivers now understand a CMD_HELLO_NEW which is
 * in sequence. Once enough of those are out in the field, we'll send
 * CMD_HELLO_NEW by default, and then phase out the old variant. */
/* Tried to remove this on 2009-08-30, but noticed that receiver was printing
 * "unexpected opcode" on retransmitted hello */
#define CMD_HELLO 0x0500

union message {
    uint16_t opCode;
    struct ok {
	uint16_t opCode;
	int16_t reserved;
	uint32_t sliceNo;
    } ok;

    struct retransmit {
	uint16_t opCode;
	int16_t reserved;
	uint32_t sliceNo;
	uint32_t rxmit;
	uint8_t map[MAX_SLICE_SIZE / BITS_PER_CHAR];
    } retransmit;

    struct connectReq {
	uint16_t opCode;
	int16_t reserved;
	uint32_t capabilities;
	uint32_t rcvbuf;
    } connectReq;

    struct go {
	uint16_t opCode;
	int16_t reserved;
    } go;

    struct disconnect {
	uint16_t opCode;
	int16_t reserved;
    } disconnect;
};



struct connectReply {
    uint16_t opCode;
    int16_t reserved;
    uint32_t clNr;
    uint32_t blockSize;
    uint32_t capabilities;
    uint8_t mcastAddr[16]; /* provide enough place for IPV6 */
};

struct hello {
    uint16_t opCode;
    int16_t reserved;
    uint32_t capabilities;
    uint8_t mcastAddr[16]; /* provide enough place for IPV6 */
    uint16_t blockSize;
};

union serverControlMsg {
    uint16_t opCode;
    int16_t reserved;
    struct hello hello;
    struct connectReply connectReply;

};


struct dataBlock {
    uint16_t opCode;
    int16_t reserved;
    uint32_t sliceNo;
    uint16_t blockNo;
    uint16_t reserved2;
    uint32_t bytes;
};

struct fecBlock {
    uint16_t opCode;
    uint16_t stripes;
    uint32_t sliceNo;
    uint16_t blockNo;
    uint16_t reserved2;
    uint32_t bytes;
};

struct reqack {
    uint16_t opCode;
    int16_t reserved;
    uint32_t sliceNo;
    uint32_t bytes;
    uint32_t rxmit;
};

union serverDataMsg {
    uint16_t opCode;
    struct reqack reqack;
    struct dataBlock dataBlock;
    struct fecBlock fecBlock;
};

/* ============================================
 * Capabilities
 */

/* Does the receiver support the new CMD_DATA command, which carries
 * capabilities mask?
 * "new generation" receiver:
 *   - capabilities word included in hello/connectReq commands
 *   - receiver multicast capable
 *   - receiver can receive ASYNC and SN
 */
#define CAP_NEW_GEN 0x0001

/* Use multicast instead of Broadcast for data */
/*#define CAP_MULTICAST 0x0002*/

#ifdef BB_FEATURE_UDPCAST_FEC
/* Forward error correction */
#define CAP_FEC 0x0004
#endif

/* Supports big endians (a.k.a. network) */
#define CAP_BIG_ENDIAN 0x0008

/* Support little endians (a.k.a. PC) ==> obsolete! */
#define CAP_LITTLE_ENDIAN 0x0010

/* This transmission is asynchronous (no receiver reply) */
#define CAP_ASYNC 0x0020

/* Sender currently supports CAPABILITIES and MULTICAST */
#define SENDER_CAPABILITIES ( \
	CAP_NEW_GEN | \
	CAP_BIG_ENDIAN)


#define RECEIVER_CAPABILITIES ( \
	CAP_NEW_GEN | \
	CAP_BIG_ENDIAN)


#endif
