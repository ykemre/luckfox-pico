#ifndef RKIPC_RTP_AUDIO_H
#define RKIPC_RTP_AUDIO_H
#include <stddef.h>
#include <stdint.h>
typedef enum { RTP_AUDIO_MP3=1, RTP_AUDIO_AAC=2, RTP_AUDIO_OPUS=3 } rtp_audio_codec;
typedef struct { uint16_t sequence; uint32_t ssrc; uint8_t payload_type; } rtp_audio_state;
typedef int (*rtp_audio_emit)(const uint8_t *packet, size_t len, void *opaque);
/* Input: one MP3 frame, raw AAC access unit, or Opus packet.
 * timestamp_us: media presentation timestamp in microseconds.
 * max_rtp_packet includes the RTP header; <= 1500.
 * Returns emitted RTP packet count, or negative on error. */
int rtp_audio_packetize(rtp_audio_state *state, rtp_audio_codec codec,
    const uint8_t *frame, size_t frame_len, uint64_t timestamp_us,
    uint32_t sample_rate, size_t max_rtp_packet,
    rtp_audio_emit emit, void *opaque);
#endif
