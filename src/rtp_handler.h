#ifndef _RTP_HANDLER_H
#define _RTP_HANDLER_H

#include <unistd.h>

#include "rtp-payload.h"
#include "rtp-profile.h"

typedef struct{
    int fd;
    void *videoEncoder;
    void *audioEncoder;
    uint32_t videoTimestamp;
    int64_t videoLastPts;
    uint32_t audioTimestamp;
    int64_t audioLastPts;
}RTP_HANDLER;


void initRtpHandler(RTP_HANDLER *rtpHandler);
void initAudioHandler(struct rtp_payload_t *audioHandler, RTP_HANDLER *rtpHandler);
void initVideoHandler(struct rtp_payload_t *videoHandler, RTP_HANDLER *rtpHandler);

#endif