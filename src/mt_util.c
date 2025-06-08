#include "mt_util.h"

void* threadRoutine(void* routineArgs) {
    RoutineArgs* args = (RoutineArgs*) routineArgs;

    while (true) {
        if (handleRequest(args->commSockFd, args->dataDir) == 0) {
            close(args->commSockFd);
            pthread_attr_destroy(args->attr);
            free(args);
            break;
        }
    }    
}


void spawnWorkerThread(int commSockFd, const char* dataDir) {
    pthread_attr_t* threadAttr = malloc(sizeof(pthread_attr_t));
    pthread_attr_init(threadAttr);
    pthread_attr_setdetachstate(threadAttr, PTHREAD_CREATE_DETACHED);

    RoutineArgs* routineArgs = malloc(sizeof(RoutineArgs));
    routineArgs->commSockFd = commSockFd;
    // no need to copy dataDir on the heap
    // pointer references a char[] allocated in main()
    routineArgs->dataDir = dataDir;
    routineArgs->attr = threadAttr;
    

    pthread_t thread;
    if (pthread_create(&thread, threadAttr, 
            &threadRoutine, routineArgs) != 0) {
        perror("Failed to spawn thread\n");
        exit(1);
    }
}

void multithreadedServer(int sockfd, const char* dataDir) {
    struct sockaddr_storage clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);

    while (true) {
        int commSockfd = accept(
            sockfd,
            (struct sockaddr*) &clientAddr,
            &clientAddrSize
        );
        if (commSockfd == -1) {
            perror("Could not accept incoming connection\n");
        }
        spawnWorkerThread(commSockfd, dataDir);
    }
}