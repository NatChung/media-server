// C program to implement one side of FIFO
// This side writes first, then reads
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>

#define MAX_BUFFER 1024*1024
#define PTS_LENGTH 8

static int gAudioPipeFd = -1;
static int gVideoPipeFd = -1;

typedef struct{
    int64_t pts;
    char data[MAX_BUFFER - PTS_LENGTH] ;
}MEDIA_FORMAT;

int sendAudioPipe(int64_t pts, void *data, int len){
    MEDIA_FORMAT audioData;
    audioData.pts = pts;
    memcpy(audioData.data, data, len);
    return write(gAudioPipeFd, &audioData, PTS_LENGTH + len);
}

int sendVideoPipe(int64_t pts, void *data, int len){
    MEDIA_FORMAT videoData;
    videoData.pts = pts;
    memcpy(videoData.data, data, len);
    return write(gVideoPipeFd, &videoData, PTS_LENGTH + len);
}

void closePipe(){
    close(gAudioPipeFd);
    close(gVideoPipeFd);
}

static int openPipe(char *fifo){
    mkfifo(fifo, 0666);
    int fd = open(fifo, O_RDWR);
    assert(fd > 0);
    return fd;
}
 
int initPipe(char *audioPipe, char *videoPipe){
    gAudioPipeFd = openPipe(audioPipe);
    gVideoPipeFd = openPipe(videoPipe);  
}