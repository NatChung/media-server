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

#define RECV_BUFFER_SIZE 32768
#define TS_PACKET_SIZE 188

unsigned short gAudioPort = 0;
unsigned short gVideoPort = 0;

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
    struct sockaddr_in addr = getLocalAddr(gAudioPort);
    sendto(rtpHandler->fd, packet, bytes, 0, (struct sockaddr *)&addr, sizeof(addr));
}

static void rtp_video_encode_packet(void* param, const void *packet, int bytes, uint32_t timestamp, int flags)
{
    RTP_HANDLER *rtpHandler = (RTP_HANDLER *)param;
    struct sockaddr_in addr = getLocalAddr(gVideoPort);
    sendto(rtpHandler->fd, packet, bytes, 0, (struct sockaddr *)&addr, sizeof(addr));
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

void initRtpHandler(RTP_HANDLER *rtpHandler){
    rtpHandler->videoTimestamp = 0;
    rtpHandler->videoLastPts = 0;
    rtpHandler->audioTimestamp = 0;
    rtpHandler->audioLastPts = 0;
}


extern int open_udp_server_socket(int *local_port);

int usage(int argc, char *argv[]){
    for(int i=1;i<argc;i++){
        char *parameter = argv[i];
        if(parameter[0] != '-') continue;

        if(strcmp(parameter, "-a") == 0){
            gAudioPort = atoi(argv[i+1]);
        }
        else if(strcmp(parameter, "-v") == 0){
            gVideoPort = atoi(argv[i+1]);
        }
    }

    if(gAudioPort == 0 || gVideoPort == 0){
        printf("Command Error !\nUsage: command -t [MpegTs source port] -a [RTP audio target port] -v [RTP video target port]\n");
        exit(0);
    }
}

int main(int argc, char *argv[]){

    struct sockaddr * from;
    socklen_t fromLen;
    char rxData[RECV_BUFFER_SIZE];
    RTP_HANDLER rtpHandler;
    int rlen, tsCount;
    int localPort = 0;
    
    usage(argc, argv);
    initRtpHandler(&rtpHandler);

    struct rtp_payload_t audioHandler;
    initAudioHandler(&audioHandler, &rtpHandler);
    struct rtp_payload_t videoHandler;
    initVideoHandler(&videoHandler, &rtpHandler);

    rtpHandler.fd = open_udp_server_socket(&localPort);
    if(rtpHandler.fd <=0){
        fprintf(stderr,"Create mpeg ts source socket failed");
        exit(0);
    }
    printf("%d", localPort);

    while(1){
        rlen = recvfrom(rtpHandler.fd, rxData, RECV_BUFFER_SIZE, 0, from, &fromLen);
        if(rlen <= 0) continue;

        for(int i=0;i<(rlen/TS_PACKET_SIZE);i++){
            mpeg_ts_packet_dec(rxData +(i*TS_PACKET_SIZE), TS_PACKET_SIZE, ts_packet, &rtpHandler);
        }
    }

    return 0;
}
