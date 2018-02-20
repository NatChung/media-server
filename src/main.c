#include <stdio.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <string.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include "rtp_handler.h"

#include "mpeg-ts.h"
#include "mpeg-ts-proto.h"


#define RECV_BUFFER_SIZE 32768
#define TS_PACKET_SIZE 188

unsigned short gAudioPort = 0;
unsigned short gVideoPort = 0;

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
