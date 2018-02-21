#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>  

#include "hls_handler.h"
#include "hls-m3u8.h"
#include "hls-media.h"
#include "hls-param.h"
#include "mpeg-ps.h"

MPEGTS_HANDLER  gMpegTsHandler;
static hls_media_t* gHLS = NULL;
static hls_m3u8_t* gM3U8 = NULL;

#define H264_SPS 7
#define H264_IDR 5
#define MY_HLS_DURATION 3


static int hls_handler(void* m3u8, const void* data, size_t bytes, int64_t pts, int64_t dts, int64_t duration)
{
    static int i = 0;
    static char name[128] = {0};
    static char plist[2 * 1024 * 1024];
    static int64_t s_dts = -1;
	int discontinue = -1 != s_dts ? 0 : (dts > s_dts + MY_HLS_DURATION / 2 ? 1 : 0);
    FILE* fp;

	s_dts = dts;

    sprintf(name, "%d.ts", i);
    hls_m3u8_add((hls_m3u8_t*)m3u8, name, pts, duration, discontinue);
    
    sprintf(name, "/home/ubuntu/s3/doorbell/test/hls/%d.ts", i++);
    fp = fopen(name, "wb");
    fwrite(data, 1, bytes, fp);
    fclose(fp);

    hls_m3u8_playlist((hls_m3u8_t*)m3u8, 0, plist, sizeof(plist));
    fp = fopen("/home/ubuntu/s3/doorbell/test/hls/index.m3u8", "wb");
    fwrite(plist, 1, strlen(plist), fp);
    fclose(fp);

    fprintf(stderr,"%s\n", plist);

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

void hlsInputH264(int64_t pts, int64_t dts, void* data, size_t bytes){
    h264_handler(pts, dts, data, bytes, isKeyFrame(data));
}

void hlsInputUlaw(int64_t pts, int64_t dts, int8_t* data, size_t bytes){
    
    for(int i=0;i<bytes;i++){
        int16_t pcm16le = MuLaw_Decode(data[i]);
        memcpy(gMpegTsHandler.pbPCMBuffer+gMpegTsHandler.nPCMBytesRead, &pcm16le, 2);
        gMpegTsHandler.nPCMBytesRead += 2;
        
        if(gMpegTsHandler.nPCMBytesRead >= gMpegTsHandler.nPCMBufferSize){
            int nRet = faacEncEncode(gMpegTsHandler.hEncoder, (int*) gMpegTsHandler.pbPCMBuffer, gMpegTsHandler.nInputSamples, gMpegTsHandler.pbAACBuffer, gMpegTsHandler.nMaxOutputBytes);
            if(nRet > 0) aac_handler(pts, dts,gMpegTsHandler.pbAACBuffer, nRet);
            gMpegTsHandler.nPCMBytesRead = 0;
        }
    }
}

void initHls(){
    initFaacHandler(&gMpegTsHandler, 8000, 1, 16);
    gM3U8 = hls_m3u8_create(0, 3);
	gHLS = hls_media_create(MY_HLS_DURATION * 1000, hls_handler, gM3U8);
}
