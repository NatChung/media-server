#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/stat.h>
#include <assert.h>
#include <string.h>

static void getTimeText(int timeValue, char *timeString){

    int hours = timeValue / 3600;
    timeValue -= hours*3600;
    int minutes = timeValue / 60;
    int seconds = timeValue - minutes * 60;
    sprintf(timeString, "%02d:%02d:%02d.000", hours, minutes, seconds);
    return timeString;
}

static void writeVtt(char *vttPath, int maxNumber){

    char startTimeString[32], endTimeString[32];
    char vtt[1024*1024]="WEBVTT";

    for(int i=1;i<=maxNumber;i++){
        int timeValue = (i-1)*2;
        getTimeText(timeValue, startTimeString);
        getTimeText(timeValue+2, endTimeString);
        sprintf(vtt, "%s\n\n%s --> %s\n/images/%d.jpg", vtt, startTimeString, endTimeString,i);
    }
    
    char vttName[1024];
    sprintf(vttName, "%s/index.vtt", vttPath);
    FILE *fp = fopen(vttName,"wb");
    fwrite(vtt, 1, strlen(vtt), fp);
    fclose(fp);
}

static int findMaxNumberOfImageFileName(char *dir){
    
    struct dirent *entry;
    struct stat statbuf;
    int maxNumber = 0;

    DIR *dp = opendir(dir);
    assert(dp!=NULL);
    chdir(dir);
    while((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name,&statbuf);
        if(!S_ISDIR(statbuf.st_mode) && atoi(entry->d_name) > maxNumber) {
            maxNumber = atoi(entry->d_name);
        }
    }
    closedir(dp);
    return maxNumber;
}


void createVttFie(char *vttPath, char *imagePath){
    int maxNumber = findMaxNumberOfImageFileName(imagePath);
    writeVtt(vttPath, maxNumber);
}