#ifndef THREADPOOL_LIBRARY_H
#define THREADPOOL_LIBRARY_H

#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <functional>

class ThreadPool {
public:
    ThreadPool();

    ~ThreadPool();

    void QueueJob(const std::function<void()> &job);

    bool isBusy();

    static void Start();

    static void Stop();

    static void QueueJob_S(const std::function<void()> &job);

    static bool isBusy_S();

private:
    void ThreadLoop();

    bool shouldStop;
    std::mutex mutex;
    std::condition_variable mutex_condition;
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> jobs;
    static ThreadPool *m_ThreadPool;
};

#endif //THREADPOOL_LIBRARY_H