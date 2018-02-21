// C program to implement one side of FIFO
// This side reads first, then reads
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/select.h>
#include <sys/time.h>

#define BUFFER_SIZE 1024 * 1024

static int openPipe(char *fifo)
{
    mkfifo(fifo, 0666);
    int fd = open(fifo, O_RDWR);
    assert(fd > 0);
    return fd;
}

int main()
{
    fd_set rfds;
    struct timeval tv;
    int rlen = -1;
    char buffer[BUFFER_SIZE];
    int videoFd = openPipe("/tmp/video");
    int audioFd = openPipe("/tmp/audio");
    int fdMax = (videoFd > audioFd) ? videoFd : audioFd;
    fprintf(stderr,"Read pipe ready\n");
    while (1)
    {
        tv.tv_sec = 1; //all these four items must be inside of the for loop
        tv.tv_usec = 0;

        FD_ZERO(&rfds);
        FD_SET(videoFd, &rfds);
        FD_SET(audioFd, &rfds);
        select(fdMax + 5, &rfds, NULL, NULL, &tv);

        if (FD_ISSET(videoFd, &rfds)){
            rlen = read(videoFd, buffer, BUFFER_SIZE);
            fprintf(stderr,"Read vidoe %d byte\n", rlen);
        }
        else if (FD_ISSET(audioFd, &rfds)){
            rlen = read(audioFd, buffer, BUFFER_SIZE);
            fprintf(stderr,"Read audio %d byte\n", rlen);
        }
    }
    return 0;
}