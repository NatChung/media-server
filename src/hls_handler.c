#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>  
#include <sys/stat.h> // mkdir

#include "hls_handler.h"
#include "hls-m3u8.h"
#include "hls-media.h"
#include "hls-param.h"
#include "mpeg-ps.h"
#include "g711.h"

FILE *outTs;
MPEGTS_HANDLER  gMpegTsHandler;
static hls_media_t* gHLS = NULL;
static hls_m3u8_t* gM3U8 = NULL;
static char *gPath = NULL;
static char *gImagePath = NULL;
static int gInotify = 1;

#define H264_SPS 7
#define H264_IDR 5
#define MY_HLS_DURATION 5


static int hls_handler(void* m3u8, const void* data, size_t bytes, int64_t pts, int64_t dts, int64_t duration)
{
    static int i = 0;
    static char name[512] = {0};
    static char plist[2 * 1024 * 1024];
    static int64_t s_dts = -1;
	int discontinue = -1 != s_dts ? 0 : (dts > s_dts + MY_HLS_DURATION / 2 ? 1 : 0);
    FILE* fp;

	s_dts = dts;

    sprintf(name, "%d.ts", i);
    hls_m3u8_add((hls_m3u8_t*)m3u8, name, pts, duration, discontinue);
    
    sprintf(name, "%s/%d.ts", gPath, i++);
    fp = fopen(name, "wb");
    fwrite(data, 1, bytes, fp);
    fclose(fp);
    // printf("TS:%s\n", name);

    hls_m3u8_playlist((hls_m3u8_t*)m3u8, 0, plist, sizeof(plist));
    // sprintf(name, "%s/index.m3u8", gPath);
    // fp = fopen(name, "wb");
    // fwrite(plist, 1, strlen(plist), fp);
    // fclose(fp);
    // printf("M3U8:%s\n", name);

    fwrite(data, 2, bytes/2, outTs);

	return 0;
}

static int aac_handler(int64_t pts, int64_t dts, void* data, size_t bytes){
    hls_media_input(gHLS, STREAM_AUDIO_AAC, data, bytes, pts, pts, 0);
}

static int h264_handler(int64_t pts, int64_t dts, void* data, size_t bytes, int flags){
    hls_media_input(gHLS, STREAM_VIDEO_H264, data, bytes, pts, pts, flags ? HLS_FLAGS_KEYFRAME : 0);
}

static int isKeyFrame(unsigned char *ptr){
    const static unsigned char startByte[4] = {0,0,0,1};
    unsigned char NALU = 0;
    if(memcmp(ptr,startByte, 4) == 0){
        NALU = (ptr[4]&0x1f);
    }else if(memcmp(ptr,startByte+1, 3) == 0){
         NALU = (ptr[3]&0x1f);
    }else{
        fprintf(stderr,"Not H264 StartByte: %02d %02d %02d %02d\n", ptr[0], ptr[1],ptr[2],ptr[3]);
        return -1;
    }
    return (NALU == H264_SPS || NALU == H264_IDR) ? 1 : 0;
}

static void initFaacHandler(MPEGTS_HANDLER *hander, ULONG nSampleRate, UINT nChannels,UINT nPCMBitSize){

    hander->nInputSamples = 0;
    hander->nMaxOutputBytes = 0;
    
    hander->hEncoder = faacEncOpen(nSampleRate, nChannels, &hander->nInputSamples, &hander->nMaxOutputBytes);
    if(hander->hEncoder == NULL){
        printf("[ERROR] Failed to call faacEncOpen()\n");
        assert(0);
    }

    hander->nPCMBufferSize = hander->nInputSamples *  nPCMBitSize / 8;
    hander->pbPCMBuffer = (BYTE *)malloc(hander->nPCMBufferSize);
    hander->pbAACBuffer = (BYTE *)malloc(hander->nMaxOutputBytes);
    hander->nPCMBytesRead = 0;

    faacEncConfigurationPtr pConfiguration = faacEncGetCurrentConfiguration(hander->hEncoder);
    pConfiguration->inputFormat = FAAC_INPUT_16BIT;
    faacEncSetConfiguration(hander->hEncoder, pConfiguration);
}

static handleAACBuffer(int64_t pts, int64_t dts, int16_t *pcm){
    memcpy(gMpegTsHandler.pbPCMBuffer+gMpegTsHandler.nPCMBytesRead, pcm, 2);
    gMpegTsHandler.nPCMBytesRead += 2;

    if(gMpegTsHandler.nPCMBytesRead >= gMpegTsHandler.nPCMBufferSize){
        int nRet = faacEncEncode(gMpegTsHandler.hEncoder, (int*) gMpegTsHandler.pbPCMBuffer, gMpegTsHandler.nInputSamples, gMpegTsHandler.pbAACBuffer, gMpegTsHandler.nMaxOutputBytes);
        if(nRet > 0) aac_handler(pts, dts,gMpegTsHandler.pbAACBuffer, nRet);
        gMpegTsHandler.nPCMBytesRead = 0;
    }
}

void hlsInputH264(int64_t pts, int64_t dts, void* data, size_t bytes){
    h264_handler(pts, dts, data, bytes, isKeyFrame(data));
}

void hlsMixUlaw(int64_t pts, int64_t dts, int8_t* data1, size_t bytes1, int8_t* data2, size_t bytes2){
    for(int i=0;i<bytes1;i++){
        int16_t pcmMixed = MuLaw_Decode(data1[i]) + ( (bytes2>0 && i<bytes2) ? MuLaw_Decode(data2[i]) : 0 );
        handleAACBuffer(pts, dts, &pcmMixed);
    }
}

void hlsInputUlaw(int64_t pts, int64_t dts, int8_t* data, size_t bytes){
    
    for(int i=0;i<bytes;i++){
        int16_t pcm16le = MuLaw_Decode(data[i]);
        memcpy(gMpegTsHandler.pbPCMBuffer+gMpegTsHandler.nPCMBytesRead, &pcm16le, 2);
        gMpegTsHandler.nPCMBytesRead += 2;
        handleAACBuffer(pts, dts, &pcm16le);
    }
}

void initOutTs(){
    
    char outTsFileName[512];
    sprintf(outTsFileName, "%s/out.ts", gPath);
    outTs = fopen(outTsFileName, "wb");
    assert(outTs!=NULL);
}


void initHls(char *recordingPath, char *imagePath){

    gPath = recordingPath; 
    gImagePath = imagePath;

    assert(mkdir(imagePath, 0777) != -1);
    printf("InitHLS Path:%s\n", gPath);

    initOutTs(recordingPath);
    initFaacHandler(&gMpegTsHandler, 8000, 1, 16);
    gM3U8 = hls_m3u8_create(0, 3);
	gHLS = hls_media_create(MY_HLS_DURATION * 1000, hls_handler, gM3U8);
}



void stopHls(){
    static char data[2 * 1024 * 1024];
    char command[1024], m3u8Path[512];

    hls_media_input(gHLS, 0, NULL, 0, 0, 0, 0);
	hls_m3u8_playlist(gM3U8, 1, data, sizeof(data));
    sprintf(m3u8Path, "%s/index.m3u8", gPath);
	FILE* fp = fopen(m3u8Path, "wb");
	fwrite(data, 1, strlen(data), fp);
	fclose(fp);

    
    hls_media_destroy(gHLS);
	hls_m3u8_destroy(gM3U8);

    fclose(outTs);
    sprintf(command, "ffmpeg -i %s/out.ts -codec copy -bsf:a aac_adtstoasc %s/index.mp4 -y -vf fps=1/2 %s/%%d.jpg", gPath, gPath, gImagePath);
    system(command);

    gInotify = 0;
}

