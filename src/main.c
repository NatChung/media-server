#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include<sys/time.h>

#include "rtp_handler.h"
#include "hls_handler.h"
#include "mpeg-ts.h"
#include "mpeg-ts-proto.h"

#define RECV_BUFFER_SIZE 32768
#define TS_PACKET_SIZE 188

unsigned short gAudioPort = 0;
unsigned short gVideoPort = 0;
long long startTime = 0;

extern int open_udp_server_socket(int *local_port);

int64_t gettime_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return (int64_t)tv.tv_sec * 1000 + (tv.tv_usec / 1000);
}

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
        rtp_payload_encode_input(rtpHandler->audioEncoder, data, bytes, createAudioTimestamp(rtpHandler, bytes));
        // hlsInputUlaw(pts/90, pts/90, data, bytes);
    }
    else if(avtype == PSI_STREAM_H264){
        rtp_payload_encode_input(rtpHandler->videoEncoder, data, bytes, createVideoTimestamp(rtpHandler, pts));
        // hlsInputH264(pts/90, pts/90, data, bytes);
    }
}

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
        printf("Command Error !\nUsage: command -a [RTP audio target port] -v [RTP video target port]\n");
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
    struct rtp_payload_t audioHandler, videoHandler;

    usage(argc, argv);

    // initHls();
    initRtpHandler(&rtpHandler);
    initAudioHandler(&audioHandler, &rtpHandler);
    initVideoHandler(&videoHandler, &rtpHandler);

    rtpHandler.fd = open_udp_server_socket(&localPort);
    if(rtpHandler.fd <=0){
        fprintf(stderr,"Create mpeg ts source socket failed");
        exit(0);
    }
    fprintf(stderr, "{\"listen\":%d}", localPort);//Must keep here for parent node js to get localPort
    
    while(1){
        rlen = recvfrom(rtpHandler.fd, rxData, RECV_BUFFER_SIZE, 0, from, &fromLen);
        if(rlen <= 0) continue;
        for(int i=0;i<(rlen/TS_PACKET_SIZE);i++){
            mpeg_ts_packet_dec(rxData +(i*TS_PACKET_SIZE), TS_PACKET_SIZE, ts_packet, &rtpHandler);
        }
    }

    return 0;
}
