// ActiveObject.h
#ifndef ACTIVE_OBJECT_H
#define ACTIVE_OBJECT_H

#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

class ActiveObject {
public:
    ActiveObject();
    ~ActiveObject();

    void send(std::function<void()> msg);
    void stop();

private:
    void run();

    std::thread th;
    std::atomic<bool> done;
    std::queue<std::function<void()>> q;
    std::mutex mtx;
    std::condition_variable cv;
};

#endif // ACTIVE_OBJECT_H
