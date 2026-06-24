#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool
{
private:
    std::vector<std::thread> workers;

    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;

    std::condition_variable condition;

    bool stop;

public:
    ThreadPool(size_t threads);

    ~ThreadPool();

    void enqueue(std::function<void()> task);
};