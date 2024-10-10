#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();

    void enqueue(std::function<void()> task);
    void stop();

private:
    void worker_thread();

    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> stop_flag;
};

#endif // THREAD_POOL_H
