#ifndef MT_UTIL_H
#define MT_UTIL_H
#include "utility.h"

typedef struct RoutineArgs {
    int commSockFd;
    const char* dataDir;
    pthread_attr_t* attr;
} RoutineArgs;

void* threadRoutine(void* args);
void spawnWorkerThread(int commSockFd, const char* dataDir);
void multithreadedServer(int sockfd, const char* dataDir);



#endif