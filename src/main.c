#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/select.h>
#include <signal.h>

#include "rtp_handler.h"
#include "mpeg-ts-proto.h"
#include "socket_handler.h"
#include "circular_queue.h"
#include "vtt_handler.h"

#define RECV_BUFFER_SIZE 32768
#define TS_PACKET_SIZE 188
#define ULAW_PACKET_SIZE 800

unsigned short gAudioPort = 0;
unsigned short gVideoPort = 0;
unsigned short gSourcePort = 0;
unsigned short g2WayAudioPort = 0;
char gRecordPath[512] = {0};
char gRecordImagePath[512] = {0};

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
        static char data2[1024];
        int byte2 = circularDeQueue(data2);
        hlsMixUlaw(pts/90, pts/90, data, bytes, data2, byte2);
        rtp_payload_encode_input(rtpHandler->audioEncoder, data, bytes, createAudioTimestamp(rtpHandler, bytes));
    }
    else if(avtype == PSI_STREAM_H264){
        hlsInputH264(pts/90, pts/90, data, bytes);
        rtp_payload_encode_input(rtpHandler->videoEncoder, data, bytes, createVideoTimestamp(rtpHandler, pts));
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
        else if(strcmp(parameter, "-s") == 0){
            gSourcePort = atoi(argv[i+1]);
        }
        else if(strcmp(parameter, "-2") == 0){
            g2WayAudioPort = atoi(argv[i+1]);
        }
        else if(strcmp(parameter, "-p") == 0){
            strcpy(gRecordPath, argv[i+1]);
            sprintf(gRecordImagePath, "%s/images", gRecordPath);
        }
    }

    if(gAudioPort == 0 || gVideoPort == 0 || gSourcePort == 0 || g2WayAudioPort == 0 || strlen(gRecordPath) == 0){
        printf("Command Error !\nUsage: command -s [MpegTs source port/TCP] -2 [2Way audio source port/TCP] -a [RTP audio target port] -v [RTP video target port] -p [Recording path]\n");
        exit(0);
    }
}

volatile sig_atomic_t runing = 1;
void sigroutine(int sig){
    runing = 0;
    stopHls();
    createVttFie(gRecordPath, gRecordImagePath);
}

int main(int argc, char *argv[]){

    struct sockaddr * from;
    socklen_t fromLen;
    char rxData[RECV_BUFFER_SIZE];
    RTP_HANDLER rtpHandler;
    int rlen, tsCount;
    int localPort = 0;
    struct rtp_payload_t audioHandler, videoHandler;
    int sockSourcefd = -1, sock2wayFd = -1, fdMax = -1;
    fd_set rfds;
    struct timeval tv;

    usage(argc, argv);
    signal(SIGHUP, sigroutine); 
    signal(SIGINT, sigroutine);
    signal(SIGQUIT, sigroutine);
    signal(SIGABRT, sigroutine);

    initHls(gRecordPath, gRecordImagePath);
    initRtpHandler(&rtpHandler);
    initAudioHandler(&audioHandler, &rtpHandler);
    initVideoHandler(&videoHandler, &rtpHandler);

    rtpHandler.fd = open_udp_server_socket(&localPort);
    sockSourcefd = connect_tcp_server_socket(gSourcePort);
    sock2wayFd = connect_tcp_server_socket(g2WayAudioPort);
    fdMax = (sockSourcefd > sock2wayFd) ? sockSourcefd : sock2wayFd;

    while (runing) {
        tv.tv_sec = 1; 
        tv.tv_usec = 0;

        FD_ZERO(&rfds);
        FD_SET(sockSourcefd, &rfds);
        FD_SET(sock2wayFd, &rfds);

        select(fdMax + 5, &rfds, NULL, NULL, &tv);
        if(runing ==0) break;

        if (FD_ISSET(sockSourcefd, &rfds)){
            rlen = recv(sockSourcefd, rxData, TS_PACKET_SIZE, 0);
            if(rlen <= 0) continue;
            assert(rlen == TS_PACKET_SIZE);
            mpeg_ts_packet_dec(rxData, TS_PACKET_SIZE, ts_packet, &rtpHandler);
        }
        else if (FD_ISSET(sock2wayFd, &rfds)){
            rlen = recv(sock2wayFd, rxData, ULAW_PACKET_SIZE, 0);
            if(rlen <= 0) continue;
            circularEnQueue(rxData, rlen);
        }
    }

    close(sock2wayFd);
    close(sockSourcefd);

    return 0;
}
