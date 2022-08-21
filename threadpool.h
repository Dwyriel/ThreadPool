#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <functional>
#include <csignal>

class ThreadPool {
public:
    ThreadPool();

    ~ThreadPool();

    template<typename Func, typename... Args>
    void QueueJob(Func job, Args &&... args);

    bool isBusy();

    void Join();

    static void Start();

    static void Stop();

    template<typename Func, typename... Args>
    static void QueueJob_S(Func &&job, Args &&... args);

    static bool isBusy_S();

private:
    void ThreadLoop();

    static void StopBySignal(int i);

    bool shouldStop;
    bool joined;
    std::mutex mutex;
    std::condition_variable mutex_condition;
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> jobs;
    static ThreadPool *m_ThreadPool;
    static bool atExitCalled;
};

#endif //THREADPOOL_H
