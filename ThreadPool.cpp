#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t numThreads) : stop_flag(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back(&ThreadPool::worker_thread, this);
    }
}

ThreadPool::~ThreadPool() {
    stop();
}

void ThreadPool::enqueue(std::function<void()> task) {
    std::lock_guard<std::mutex> lock(mtx);
    if (stop_flag) {
        throw std::runtime_error("enqueue on stopped ThreadPool");
    }
    tasks.emplace(std::move(task));
    cv.notify_one();
}

void ThreadPool::stop() {
    {
        std::unique_lock<std::mutex> lock(mtx);
        stop_flag = true;
        cv.notify_all();
    }
    for (std::thread &worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::worker_thread() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this]() { return stop_flag || !tasks.empty(); });
            if (stop_flag && tasks.empty()) {
                return;
            }
            if (!tasks.empty()) {
                task = std::move(tasks.front());
                tasks.pop();
            }
        }
        if (task) {
            task();
        }
    }
}