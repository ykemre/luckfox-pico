#include "rtp_audio_sdp.h"
#include <stdio.h>
#include <string.h>

int rtp_audio_sdp(char *out, size_t out_size, rtp_audio_codec codec,
                  unsigned payload_type, unsigned sample_rate,
                  unsigned channels, const uint8_t *aac_config,
                  size_t aac_config_len)
{
    int n;
    char hex[129];
    size_t i;
    if (!out || out_size==0 || channels<1 || channels>2 ||
        payload_type>127 || sample_rate==0) return -1;
    if (codec==RTP_AUDIO_MP3) {
        /* RFC 3551 static payload type 14: MPA/90000. */
        if (payload_type!=14) return -1;
        n=snprintf(out,out_size,"m=audio 0 RTP/AVP 14\r\na=rtpmap:14 MPA/90000\r\na=control:track2\r\n");
    } else if (codec==RTP_AUDIO_OPUS) {
        if (payload_type<96) return -1;
        n=snprintf(out,out_size,
            "m=audio 0 RTP/AVP %u\r\na=rtpmap:%u opus/48000/2\r\n"
            "a=fmtp:%u sprop-stereo=%u\r\na=control:track2\r\n",
            payload_type,payload_type,payload_type,channels==2?1u:0u);
    } else if (codec==RTP_AUDIO_AAC) {
        if (payload_type<96 || !aac_config || !aac_config_len ||
            aac_config_len>64) return -1;
        for (i=0;i<aac_config_len;i++)
            snprintf(hex+i*2,sizeof(hex)-i*2,"%02X",aac_config[i]);
        hex[aac_config_len*2]=0;
        n=snprintf(out,out_size,
            "m=audio 0 RTP/AVP %u\r\na=rtpmap:%u MPEG4-GENERIC/%u/%u\r\n"
            "a=fmtp:%u streamtype=5; profile-level-id=1; mode=AAC-hbr; "
            "config=%s; SizeLength=13; IndexLength=3; IndexDeltaLength=3\r\n"
            "a=control:track2\r\n",
            payload_type,payload_type,sample_rate,channels,payload_type,hex);
    } else return -1;
    return n<0 || (size_t)n>=out_size ? -1 : n;
}
