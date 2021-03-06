#ifndef _HLS_HANDLER_H
#define _HLS_HANDLER_H

#include <unistd.h>
#include "faac.h"

typedef unsigned long   ULONG;
typedef unsigned int    UINT;
typedef unsigned char   BYTE;
typedef char            _TCHAR;
typedef struct{
    faacEncHandle hEncoder;
    ULONG nMaxOutputBytes;
    ULONG nInputSamples;
    UINT nPCMBufferSize;
    UINT nPCMBytesRead;
    BYTE* pbPCMBuffer;
    BYTE* pbAACBuffer;
}MPEGTS_HANDLER;

void initHls(char *recordingPath, char *imagePath);
void stopHls();
void hlsInputH264(int64_t pts, int64_t dts, void* data, size_t bytes);
void hlsInputUlaw(int64_t pts, int64_t dts, int8_t* data, size_t bytes);
void hlsMixUlaw(int64_t pts, int64_t dts, int8_t* data1, size_t bytes1, int8_t* data2, size_t bytes2);

#endif