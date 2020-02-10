#include "net/Server.h"

#include <iostream>

void Server::Start() {
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(serverPort_);

    bind(listenFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    listen(listenFd, 1);
    while (true) {
        int connectFd = accept(listenFd, (struct sockaddr*)NULL, NULL);

        threadPool_.Submit(TaskType([&](int connectFd) {
            std::cout << "hello\n";
        }, connectFd));
    }
}

void Server::Stop() {
    threadPool_.Stop();
    close(serverPort_);
}