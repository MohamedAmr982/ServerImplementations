#include "mp_util.h"

void setNonblocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFD, 0);
    if (flags == -1) {
        perror("Could not get socket flags");
        exit(1);
    }
    if (fcntl(sockfd, F_SETFD, flags | O_NONBLOCK) == -1) {
        perror("Could not modify socket\n");
        exit(1);
    }
}

void registerReadEvent(int epollfd, int sockfd) {
    // register a read event from sockfd in the epoll 
    // interest list
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.data.fd = sockfd;
    // read event, edge triggered
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &event) == -1) {
        perror("Could not register socket read event\n");
        exit(1);
    }
}

void removeReadEvent(int epollfd, struct epoll_event* event) {
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, 
            event->data.fd, event) == -1) {
        perror("Could not remove event\n");
        exit(1);
    }
}

void* acceptorRoutine(void* acceptorArgs) {
    // wait for new connections
    // accept them, set their sockets as nonblocking
    // and register them in the epoll interest list

    AcceptorArgs* args = (AcceptorArgs*) acceptorArgs;
    int epollfd = args->epollfd;
    int sockfd = args->sockfd;

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
        setNonblocking(commSockfd);
        registerReadEvent(epollfd, commSockfd);
    }
}

void* workerRoutine(void* workerArgs) {
    // wait for ready events
    // read as much as bytes as possible
    // return response

    WorkerArgs* args = (WorkerArgs*) workerArgs;

    struct epoll_event events[args->queueLen];

    while (true) {
        int eventCount = epoll_wait(
            args->epollfd,
            events,
            args->queueLen,
            -1
        );
        if (eventCount == -1) {
            perror("epoll_wait() error ");
            perror(strerror(errno));
            perror("\n");
            exit(1);
        }
        for (int i = 0; i < eventCount; i++) {
            if (
                events[i].events & (EPOLLERR | EPOLLHUP) ||
                !(events[i].events & EPOLLIN)
            ) {
                close(events[i].data.fd);
                continue;
            } else {
                if (handleRequest(events[i].data.fd, args->dataDir) == 0) {
                    removeReadEvent(args->epollfd, events + i);
                    close(events[i].data.fd);
                }
            }
        }
    }
}

void multiprocessServer(int sockfd, const char* dataDir, int queueLen) {
    // set epoll fd
    int epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("Could not get epoll fd\n");
        exit(1);
    }
    
    // dispatch acceptor and worker threads
    pthread_t acceptor, worker;
    
    AcceptorArgs acceptorArgs = {
        .epollfd = epollfd,
        .sockfd = sockfd
    };
    if (pthread_create(&acceptor, NULL, 
            &acceptorRoutine, &acceptorArgs) != 0) {
        perror("Failed to dispatch acceptor thread\n");
        exit(1);
    }

    WorkerArgs workerArgs = {
        .dataDir = dataDir,
        .epollfd = epollfd,
        .queueLen = queueLen,
        .sockfd = sockfd
    };
    if (pthread_create(&worker, NULL, 
            &workerRoutine, &workerArgs) != 0) {
        perror("Failed to dispatch worker thread\n");
        exit(1);
    }
    
    pthread_join(acceptor, NULL);
    pthread_join(worker, NULL);
}
