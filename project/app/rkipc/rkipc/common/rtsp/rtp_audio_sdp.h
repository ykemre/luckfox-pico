#ifndef RKIPC_RTP_AUDIO_SDP_H
#define RKIPC_RTP_AUDIO_SDP_H
#include <stddef.h>
#include <stdint.h>
#include "rtp_audio.h"
/* Write audio media section for SDP; returns bytes written or -1.
 * AAC config is MPEG-4 AudioSpecificConfig, normally 2 bytes.
 * MP3 uses RFC 2250 static payload type 14 (90000 Hz clock).
 * Opus RTP clock is always 48000 Hz, with 1 or 2 channels. */
int rtp_audio_sdp(char *out, size_t out_size, rtp_audio_codec codec,
                  unsigned payload_type, unsigned sample_rate,
                  unsigned channels, const uint8_t *aac_config,
                  size_t aac_config_len);
#endif
