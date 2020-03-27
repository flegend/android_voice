#ifndef MY_VOICE_ENGINE_TYPES_H
#define MY_VOICE_ENGINE_TYPES_H

#include <stddef.h>

typedef unsigned char uint8_t;
//typedef unsigned long size_t;
enum VOICE_ENGINE_CODEC_ID
{
    CODEC_PCMA = 0,         //8K
    CODEC_PCMU,				//8K
    CODEC_GSM0610,          //8K
    CODEC_G722,				//8K
    CODEC_G7221_24K,		//16K
    CODEC_G7221_32K,		//16K
    CODEC_G7221C_24K,		//32K
    CODEC_G7221C_32K,		//32K
    CODEC_G7221C_48K,		//32K
    CODEC_OPUS,				//48K

    CODEC_INVALID
};

// EC modes
enum EcModes                   // type of Echo Control
{
    kEcUnchanged = 0,          // previously set mode
    kEcDefault,                // platform default
    kEcConference,             // conferencing default (aggressive AEC)
    kEcAec,                    // Acoustic Echo Cancellation
    kEcAecm,                   // AEC mobile
    kEcSpxAec,
};

// AECM modes
enum AecmModes                 // mode of AECM
{
    kAecmQuietEarpieceOrHeadset = 0,
    // Quiet earpiece or headset use
    kAecmEarpiece,             // most earpiece use
    kAecmLoudEarpiece,         // Loud earpiece or quiet speakerphone use
    kAecmSpeakerphone,         // most speakerphone use (default)
    kAecmLoudSpeakerphone      // Loud speakerphone
};

enum NsModes    // type of Noise Suppression
{
    kNsUnchanged = 0,   // previously set mode
    kNsDefault,         // platform default
    kNsConference,      // conferencing default
    kNsLowSuppression,  // lowest suppression
    kNsModerateSuppression,
    kNsHighSuppression,
    kNsVeryHighSuppression,     // highest suppression
};

enum AgcModes                  // type of Automatic Gain Control
{
    kAgcUnchanged = 0,        // previously set mode
    kAgcDefault,              // platform default
    // adaptive mode for use when analog volume control exists (e.g. for
    // PC softphone)
    kAgcAdaptiveAnalog,
    // scaling takes place in the digital domain (e.g. for conference servers
    // and embedded devices)
    kAgcAdaptiveDigital,
    // can be used on embedded devices where the capture signal level
    // is predictable
    kAgcFixedDigital
};

// AGC configuration
typedef struct
{
    unsigned short targetLeveldBOv;				//[0,31]
    unsigned short digitalCompressionGaindB;	//[0,90]
    bool           limiterEnable;
} AgcConfig;                  // AGC configuration parameters

enum TraceLevel
{
    kTraceNone = 0x0000,    // no trace
    kTraceStateInfo = 0x0001,
    kTraceWarning = 0x0002,
    kTraceError = 0x0004,
    kTraceCritical = 0x0008,
    kTraceApiCall = 0x0010,
    kTraceDefault = 0x00ff,

    kTraceModuleCall = 0x0020,
    kTraceMemory = 0x0100,   // memory info
    kTraceTimer = 0x0200,   // timing info
    kTraceStream = 0x0400,   // "continuous" stream of data

    // used for debug purposes
    kTraceDebug = 0x0800,  // debug
    kTraceInfo = 0x1000,  // debug info

    // Non-verbose level used by LS_INFO of logging.h. Do not use directly.
    kTraceTerseInfo = 0x2000,

    kTraceAll = 0xffff
};

// Each codec supported can be described by this structure.
struct CodecInst {
    int pltype;
    char plname[32];
    int plfreq;
    int pacsize;
    int channels;
    int rate;  // bits/sec unlike {start,min,max}Bitrate elsewhere in this file!
};

class TraceCallback {
public:
    virtual void Print(TraceLevel level, const char* message, int length) = 0;

protected:
    virtual ~TraceCallback() {}
    TraceCallback() {}
};

// External transport callback interface
class Transport
{
public:
    virtual int SendPacket(int channel, const void *data, size_t len) = 0;
    virtual int SendRTCPPacket(int channel, const void *data, size_t len) = 0;

protected:
    virtual ~Transport() {}
    Transport() {}
};

#endif // MY_VOICE_ENGINE_TYPES_H

