#include "rtp_audio.h"
#include <string.h>

#define RTP_HEADER_SIZE 12u
#define RTP_AUDIO_PACKET_CAP 1500u

static void put16(uint8_t *p, uint16_t v) { p[0]=(uint8_t)(v>>8); p[1]=(uint8_t)v; }
static void put32(uint8_t *p, uint32_t v) {
    p[0]=(uint8_t)(v>>24); p[1]=(uint8_t)(v>>16);
    p[2]=(uint8_t)(v>>8); p[3]=(uint8_t)v;
}

/* RFC 2250 (MPA), RFC 3640 (MPEG4-GENERIC), RFC 7587 (Opus).
 * Packetizer only: caller must supply encoded frames and SDP/RTSP setup. */
int rtp_audio_packetize(rtp_audio_state *state, rtp_audio_codec codec,
                        const uint8_t *frame, size_t frame_len,
                        uint64_t timestamp_us, uint32_t sample_rate,
                        size_t max_rtp_packet, rtp_audio_emit emit, void *opaque)
{
    uint8_t packet[RTP_AUDIO_PACKET_CAP];
    size_t offset=0, extra, capacity;
    uint32_t clock_rate, rtp_timestamp;
    int count=0;
    if (!state || !frame || !frame_len || !emit || !sample_rate ||
        max_rtp_packet>sizeof(packet) || max_rtp_packet<=RTP_HEADER_SIZE)
        return -1;
    if (codec!=RTP_AUDIO_MP3 && codec!=RTP_AUDIO_AAC && codec!=RTP_AUDIO_OPUS)
        return -1;
    if (codec==RTP_AUDIO_MP3 && frame_len>65535u) return -1;
    if (codec==RTP_AUDIO_AAC && frame_len>8191u) return -1;
    extra=(codec==RTP_AUDIO_MP3 || codec==RTP_AUDIO_AAC)?4u:0u;
    if (max_rtp_packet<=RTP_HEADER_SIZE+extra) return -1;
    capacity=max_rtp_packet-RTP_HEADER_SIZE-extra;
    if (codec==RTP_AUDIO_OPUS && frame_len>capacity) return -1;
    clock_rate=codec==RTP_AUDIO_MP3?90000u:
               codec==RTP_AUDIO_OPUS?48000u:sample_rate;
    rtp_timestamp=(uint32_t)((timestamp_us*clock_rate)/1000000u);
    while (offset<frame_len) {
        size_t chunk=frame_len-offset;
        int final;
        if (chunk>capacity) chunk=capacity;
        final=(offset+chunk==frame_len);
        packet[0]=0x80;
        packet[1]=(uint8_t)((final?0x80:0)|(state->payload_type&0x7f));
        put16(packet+2,state->sequence);
        put32(packet+4,rtp_timestamp);
        put32(packet+8,state->ssrc);
        if (codec==RTP_AUDIO_MP3) {
            packet[12]=packet[13]=0;
            put16(packet+14,(uint16_t)offset);
        } else if (codec==RTP_AUDIO_AAC) {
            packet[12]=0; packet[13]=16;
            packet[14]=(uint8_t)(frame_len>>5);
            packet[15]=(uint8_t)((frame_len&31u)<<3);
        }
        memcpy(packet+RTP_HEADER_SIZE+extra,frame+offset,chunk);
        if (emit(packet,RTP_HEADER_SIZE+extra+chunk,opaque)!=0) return -2;
        state->sequence++;
        offset+=chunk;
        count++;
    }
    return count;
}
