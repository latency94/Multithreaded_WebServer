#pragma once

#include "ThreadPool.h"
#include <atomic>

class Server
{
private:
    int serverSocket;
    int port;

    ThreadPool pool;

    std::atomic<int> requestCount;

    void handleClient(int clientSocket);

public:
    Server(int p);

    bool start();

    void run();
};