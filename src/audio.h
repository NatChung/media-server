#ifndef _AUDIO_H
#define _AUDIO_H
int SendAudioData(int fd, char * audioData, int channel_number);
int SendAudioCommand(int fd, char audioOn ,int channel_number);
#endif