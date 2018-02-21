#ifndef _PIPE_HANDLER_H
#define _PIPE_HANDLER_H

int sendAudioPipe(unsigned char *data, int len);
int sendVideoPipe(unsigned char *data, int len);
void closePipe();
int initPipe(char *myfifo);

#endif