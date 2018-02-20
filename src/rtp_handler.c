#include "rtp_handler.h"

#include <netinet/in.h>
#include <assert.h>

extern unsigned short gAudioPort;
extern unsigned short gVideoPort;

static struct sockaddr_in getLocalAddr(unsigned short port){
    struct sockaddr_in addr;
    addr.sin_family = AF_INET ;
	addr.sin_addr.s_addr = 0x0100007f;
	addr.sin_port = htons(port) ;
    return addr;
}

static void* rtp_alloc(void* param, int bytes)
{
	static uint8_t buffer[2 * 1024 * 1024 + 4] = { 0, 0, 0, 1, };
	assert(bytes <= sizeof(buffer) - 4);
	return buffer + 4;
}

static void rtp_free(void* param, void * packet)
{
}

static void rtp_audio_encode_packet(void* param, const void *packet, int bytes, uint32_t timestamp, int flags)
{
    RTP_HANDLER *rtpHandler = (RTP_HANDLER *)param;
    struct sockaddr_in addr = getLocalAddr(gAudioPort);
    sendto(rtpHandler->fd, packet, bytes, 0, (struct sockaddr *)&addr, sizeof(addr));
}

static void rtp_video_encode_packet(void* param, const void *packet, int bytes, uint32_t timestamp, int flags)
{
    RTP_HANDLER *rtpHandler = (RTP_HANDLER *)param;
    struct sockaddr_in addr = getLocalAddr(gVideoPort);
    sendto(rtpHandler->fd, packet, bytes, 0, (struct sockaddr *)&addr, sizeof(addr));
}

void initRtpHandler(RTP_HANDLER *rtpHandler){
    rtpHandler->videoTimestamp = 0;
    rtpHandler->videoLastPts = 0;
    rtpHandler->audioTimestamp = 0;
    rtpHandler->audioLastPts = 0;
}

void initAudioHandler(struct rtp_payload_t *audioHandler, RTP_HANDLER *rtpHandler){
    audioHandler->alloc = rtp_alloc;
	audioHandler->free = rtp_free;
	audioHandler->packet = rtp_audio_encode_packet;
    rtpHandler->audioEncoder = rtp_payload_encode_create(RTP_PAYLOAD_PCMU, "PCMU", 1, gAudioPort, audioHandler, rtpHandler);
}

void initVideoHandler(struct rtp_payload_t *videoHandler, RTP_HANDLER *rtpHandler){
    videoHandler->alloc = rtp_alloc;
	videoHandler->free = rtp_free;
	videoHandler->packet = rtp_video_encode_packet;
    rtpHandler->videoEncoder = rtp_payload_encode_create(RTP_PAYLOAD_H264, "H264", 1, gVideoPort, videoHandler, rtpHandler);
}