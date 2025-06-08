#include "server.h"


void singleThreadedServer(int sockfd, const char* dataDir) {
    struct sockaddr_storage clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);

    while (true) {
        int commSockfd;
        if ((commSockfd = accept(sockfd, 
            (struct sockaddr*) &clientAddr, &clientAddrSize)) == -1) {
            perror("Could not accept incoming request.\n");
        }

        handleRequest(commSockfd, dataDir);
        close(commSockfd);
    }
}

void mainLoop(int sockfd, int queueLen, const char* dataDir, ServerMode mode) {
    
    if (listen(sockfd, queueLen) == -1) {
        perror("Could not listen to port.\n");
        exit(2);
    }

    switch (mode) {
        case SINGLE_THREADED:
            singleThreadedServer(sockfd, dataDir);
            break;
        case MULTIPROCESS:
            multiprocessServer(sockfd, dataDir, queueLen);
            break;
        case MULTITHREADED:
            multithreadedServer(sockfd, dataDir);
            break;
    }
}
