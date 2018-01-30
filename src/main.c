#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <assert.h>

#include "mpeg-ts.h"
#include "rtp-payload.h"
#include "mpeg-ts-proto.h"
#include "rtp-profile.h"

#include "b64.h"
#include "audio.h"


#define RECV_BUFFER_SIZE 32768
#define TS_PACKET_SIZE 188

#define GSSC_STREAM_PORT    12345
#define RTP_PORT            10000
#define AUDIO_BRIDGE_PORT   43347

typedef struct{
    int fd;
    void *videoEncoder;
    void *audioEncoder;
    uint32_t videoTimestamp;
    int64_t videoLastPts;
    uint32_t audioTimestamp;
    int64_t audioLastPts;
}RTP_HANDLER;

uint32_t createVideoTimestamp(RTP_HANDLER *rtpHandler, int64_t pts){
    rtpHandler->videoTimestamp += pts - rtpHandler->videoLastPts;
    rtpHandler->videoLastPts = pts;
    return rtpHandler->videoTimestamp;
}

uint32_t createAudioTimestamp(RTP_HANDLER *rtpHandler, size_t bytes){
    rtpHandler->audioTimestamp += bytes;
    return rtpHandler->audioTimestamp;
}

#define BUFFER 2048
unsigned char audioBuffer[BUFFER] = {0};
int readIndex = 0, writeIndex = 0, count = 0;

static void ts_packet(void* param, int avtype, int64_t pts, int64_t dts, void* data, size_t bytes)
{
    RTP_HANDLER *rtpHandler = (RTP_HANDLER *)param;

    if(avtype == PSI_STREAM_PRIVATE_DATA){
        int timestamp = createAudioTimestamp(rtpHandler, bytes) ;//The pts for each is 9000, but it is incorrect timestamp. should as same as ulaw size
        rtp_payload_encode_input(rtpHandler->audioEncoder, data, bytes, timestamp);
    }
    else if(avtype == PSI_STREAM_H264){
        int timestamp = createVideoTimestamp(rtpHandler, pts);
        rtp_payload_encode_input(rtpHandler->videoEncoder, data, bytes, timestamp);
    }

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

static struct sockaddr_in getLocalAddr(unsigned short port){
     struct sockaddr_in addr;
    addr.sin_family = AF_INET ;
	addr.sin_addr.s_addr = 0x0100007f;
	addr.sin_port = htons(port) ;
    return addr;
}

static void rtp_audio_encode_packet(void* param, const void *packet, int bytes, uint32_t timestamp, int flags)
{
    RTP_HANDLER *rtpHandler = (RTP_HANDLER *)param;
    struct sockaddr_in addr = getLocalAddr(22347);
    sendto(rtpHandler->fd, packet, bytes, 0, (struct sockaddr *)&addr, sizeof(addr));
}

static void rtp_video_encode_packet(void* param, const void *packet, int bytes, uint32_t timestamp, int flags)
{
    RTP_HANDLER *rtpHandler = (RTP_HANDLER *)param;
    struct sockaddr_in addr = getLocalAddr(12347);
    if(bytes > 1450)
        printf("\n Sent bytes = %d", bytes);
    sendto(rtpHandler->fd, packet, bytes, 0, (struct sockaddr *)&addr, sizeof(addr));
}


void initAudioHandler(struct rtp_payload_t *audioHandler, RTP_HANDLER *rtpHandler){
    audioHandler->alloc = rtp_alloc;
	audioHandler->free = rtp_free;
	audioHandler->packet = rtp_audio_encode_packet;
    rtpHandler->audioEncoder = rtp_payload_encode_create(RTP_PAYLOAD_PCMU, "PCMU", 1, 1724854, audioHandler, rtpHandler);
}

void initVideoHandler(struct rtp_payload_t *videoHandler, RTP_HANDLER *rtpHandler){
    videoHandler->alloc = rtp_alloc;
	videoHandler->free = rtp_free;
	videoHandler->packet = rtp_video_encode_packet;
    rtpHandler->videoEncoder = rtp_payload_encode_create(RTP_PAYLOAD_H264, "H264", 1, 621539, videoHandler, rtpHandler);
}

void initRtpHandler(RTP_HANDLER *rtpHandler){
    rtpHandler->videoTimestamp = 0;
    rtpHandler->videoLastPts = 0;
    rtpHandler->audioTimestamp = 0;
    rtpHandler->audioLastPts = 0;
}


extern int open_udp_server_socket(int local_port);
int main(int argc, char *argv[]){

    struct sockaddr * from;
    socklen_t fromLen;
    char rxData[RECV_BUFFER_SIZE], audioData[RECV_BUFFER_SIZE];
    RTP_HANDLER rtpHandler;
    struct timeval tv;
    int fd_max, rtpFd, audioBridgeFd;
    fd_set rfds;

    initRtpHandler(&rtpHandler);

    // FILE *f = fopen("/Users/jeffrey/Desktop/audio.mulaw", "w+");
    // if (f == NULL)
    // {
    //     printf("Error opening file!\n");
    //     exit(1);
    // }

    struct rtp_payload_t audioHandler;
    initAudioHandler(&audioHandler, &rtpHandler);

    struct rtp_payload_t videoHandler;
    initVideoHandler(&videoHandler, &rtpHandler);

    rtpHandler.fd = open_udp_server_socket(GSSC_STREAM_PORT);
    rtpFd = open_udp_server_socket(RTP_PORT);
    fd_max = (rtpFd > rtpHandler.fd) ? rtpFd : rtpHandler.fd;

    audioBridgeFd = open_udp_server_socket(AUDIO_BRIDGE_PORT);
    fd_max = (fd_max > audioBridgeFd) ? fd_max : audioBridgeFd;

    SendAudioCommand(audioBridgeFd, 1, 0);

   
    
    while(1)
    {
        tv.tv_sec = 1 ;         //all these four items must be inside of the for loop
        tv.tv_usec = 0 ;
        
        FD_ZERO(&rfds);
        FD_SET(rtpHandler.fd, &rfds);
        FD_SET(rtpFd, &rfds);
        FD_SET(audioBridgeFd, &rfds);
        
        if( select(fd_max +5, &rfds, NULL, NULL, &tv) > 0 )
        {
            if(FD_ISSET(rtpHandler.fd, &rfds) )
            {
                fromLen = sizeof(from);
                int rlen = recvfrom(rtpHandler.fd, rxData, RECV_BUFFER_SIZE, 0, from, &fromLen);
                int tsCount = rlen/TS_PACKET_SIZE;
                for(int i=0;i<tsCount;i++){
                    mpeg_ts_packet_dec(rxData +(i*TS_PACKET_SIZE), TS_PACKET_SIZE, ts_packet, &rtpHandler);
                }
            }
            else if(FD_ISSET(rtpFd, &rfds)){
                fromLen = sizeof(from);
                int rlen = recvfrom(rtpFd, rxData, RECV_BUFFER_SIZE, 0, from, &fromLen);
                printf("\n Data port/1000, rlen=%d", rlen);
            }
            else if(FD_ISSET(audioBridgeFd, &rfds)){
                fromLen = sizeof(from);
                int rlen = recvfrom(audioBridgeFd, rxData, RECV_BUFFER_SIZE, 0, from, &fromLen);
                if(rlen <= 0) continue;

                size_t decodeLen = 0;
                char *adata = b64_decode_ex(rxData, rlen, &decodeLen);
                printf("dlen = %d\n", decodeLen);
                SendAudioData(audioBridgeFd, adata, 0);
                //fwrite(adata,1,800,f);
            }
        }
    }

    return 0;
}