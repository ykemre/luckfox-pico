#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../rtp_audio.h"
#include "../rtp_audio_sdp.h"

struct capture { unsigned count; size_t bytes; uint8_t first[1500]; };
static int capture_packet(const uint8_t *p, size_t n, void *opaque) {
    struct capture *c=(struct capture *)opaque;
    if (!c->count) memcpy(c->first,p,n);
    c->count++; c->bytes+=n;
    return 0;
}
int main(void) {
    uint8_t frame[3000]={0};
    uint8_t asc[2]={0x14,0x08}; /* AAC-LC 16000 Hz mono */
    char sdp[512];
    struct capture c={0};
    rtp_audio_state st={.sequence=10,.ssrc=0x12345678,.payload_type=14};
    assert(rtp_audio_sdp(sdp,sizeof(sdp),RTP_AUDIO_MP3,14,16000,1,NULL,0)>0);
    assert(strstr(sdp,"MPA/90000"));
    assert(rtp_audio_packetize(&st,RTP_AUDIO_MP3,frame,sizeof(frame),1000000,
                               16000,1200,capture_packet,&c)==3);
    assert(c.count==3 && st.sequence==13);
    assert(c.first[1]==14 && c.first[4]==0x00 && c.first[5]==0x01 &&
           c.first[6]==0x5f && c.first[7]==0x90);
    memset(&c,0,sizeof(c));
    st.payload_type=97;
    assert(rtp_audio_sdp(sdp,sizeof(sdp),RTP_AUDIO_AAC,97,16000,1,asc,2)>0);
    assert(strstr(sdp,"config=1408"));
    assert(rtp_audio_packetize(&st,RTP_AUDIO_AAC,frame,100,0,
                               16000,1200,capture_packet,&c)==1);
    assert(c.first[12]==0 && c.first[13]==16);
    memset(&c,0,sizeof(c));
    assert(rtp_audio_sdp(sdp,sizeof(sdp),RTP_AUDIO_OPUS,97,16000,1,NULL,0)>0);
    assert(strstr(sdp,"opus/48000/2"));
    assert(rtp_audio_packetize(&st,RTP_AUDIO_OPUS,frame,80,1000000,
                               16000,1200,capture_packet,&c)==1);
    assert(c.first[4]==0 && c.first[5]==0 && c.first[6]==0xbb && c.first[7]==0x80);
    puts("RTP audio/SDP tests passed");
    return 0;
}
