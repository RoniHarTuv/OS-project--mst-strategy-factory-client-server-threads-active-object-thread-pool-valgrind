#include "ActiveObject.h"

ActiveObject::ActiveObject() : done(false) {
    th = std::thread(&ActiveObject::run, this);
}

ActiveObject::~ActiveObject() {
    stop();
}

void ActiveObject::send(std::function<void()> msg) {
    std::lock_guard<std::mutex> lock(mtx);
    q.push(std::move(msg));
    cv.notify_one();
}

void ActiveObject::stop() {
    std::unique_lock<std::mutex> lock(mtx);
    done = true;
    cv.notify_all();
    lock.unlock();
    if (th.joinable()) {
        th.join();
    }
}

void ActiveObject::run() {
    while (true) {
        std::function<void()> msg;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this]() { return !q.empty() || done; });
            if (done && q.empty()) break;
            if (!q.empty()) {
                msg = std::move(q.front());
                q.pop();
            }
        }
        if (msg) msg();
    }
}