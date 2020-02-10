#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "common/ThreadPool.h"

class Server {
    ThreadPool<TaskType> threadPool_;
    int serverPort_;
public:
    Server(int serverPort = 8888) : serverPort_(serverPort) {}

    void Start();

    void Stop();

    ~Server() {}
};

#endif