#ifndef RTP_HEADER_H
#define RTP_HEADER_H

#ifdef RTP_VERSION
#undef RTP_VERSION
#endif
#define RTP_VERSION			0x02

#ifdef RTP_HEADER_LEN
#undef RTP_HEADER_LEN
#endif
#define RTP_HEADER_LEN		0x0C

/// RTP packet header
typedef struct tagRTP_HEADER
{
    unsigned short count:4;              // csrc count
    unsigned short extension:1;          // header extension flag
    unsigned short padding:1;            // padding flag - for encryption
    unsigned short version:2;            // protocol version


    unsigned short type:7;               // payload type
    unsigned short marker:1;             // marker bit - for profile

    unsigned short sequence;          // sequence number of this packet 
    // (needs endian conversion here, 
    // done in the access methods )

    /// timestamp of this packet
    unsigned long timestamp;
    /// source of packet
    unsigned long ssrc;
}RTP_HEADER,*PRTP_HEADER;

#endif
