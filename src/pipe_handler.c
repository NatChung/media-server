// C program to implement one side of FIFO
// This side writes first, then reads
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

static int gAudioPipeFd = -1;
static int gVideoPipeFd = -1;

int sendAudioPipe(unsigned char *data, int len){
    return write(gAudioPipeFd, data, len);
}

int sendVideoPipe(unsigned char *data, int len){
    return write(gVideoPipeFd, data, len);
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
 
int initPipe(char *myfifo){
    char piep[128] = {0};

    sprintf(piep, "%s/audio", myfifo);
    gAudioPipeFd = openPipe(piep);
    sprintf(piep, "%s/video", myfifo);
    gVideoPipeFd = openPipe(piep);  
}