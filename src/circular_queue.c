#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define MAX_SIZE 1024
#define MAX_NODE 5

typedef struct{
    size_t len;
    char data[MAX_SIZE];
}NODE;

static NODE gBuffer[MAX_NODE];
static unsigned int gRead = 0, gWrite = 0, gCount = 0;

int circularEnQueue(const void *inBuffer, const size_t inLen){
    assert(inLen <= MAX_SIZE);
    if(gCount>=MAX_NODE) return -1;//FULL

    NODE *ptr = gBuffer + (gWrite % MAX_NODE);
    ptr->len = inLen;
    memcpy(ptr->data, inBuffer, inLen);
    ++gWrite; ++gCount;
    return gCount;
}

size_t circularDeQueue(char *outBuffer){
    if(gCount<=0) return 0;//EMPTY

    NODE *ptr = gBuffer + (gRead % MAX_NODE);
    memcpy(outBuffer, ptr->data, ptr->len);
    ++gRead; --gCount;
    return ptr->len;
}