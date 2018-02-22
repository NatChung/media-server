#ifndef _PIPE_HANDLER_H
#define _PIPE_HANDLER_H


#include <unistd.h>

int sendAudioPipe(int64_t pts, void *data, int len);
int sendVideoPipe(int64_t pts, void *data, int len);
void closePipe();
int initPipe(char *audioPipe, char *videoPipe);

#endif