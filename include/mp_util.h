#ifndef MP_UTIL_H
#define MP_UTIL_H

#include "utility.h"


typedef struct AcceptorArgs {
    int epollfd;
    int sockfd;
} AcceptorArgs;

typedef struct WorkerArgs {
    int sockfd;
    int epollfd;
    const char* dataDir;
    int queueLen;
} WorkerArgs;

void multiprocessServer(int sockfd, const char* dataDir, int queueLen);

void* acceptorRoutine(void* acceptorArgs);

void* workerRoutine(void* workerArgs);

void setNonblocking(int sockfd);

void registerReadEvent(int epollfd, int sockfd);

void removeReadEvent(int epollfd, struct epoll_event* event);


#endif