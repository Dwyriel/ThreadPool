#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <functional>
//#include <csignal>

class ThreadPool {
public:
    ThreadPool();

    ~ThreadPool();

    template<typename Func, typename... Args>
    void QueueJob(Func job, Args &&... args);

    bool isBusy();

    void Join();

    void Stop();

    static void Start_S();

    static void Stop_S();

    template<typename Func, typename... Args>
    static void QueueJob_S(Func &&job, Args &&... args);

    static bool isBusy_S();

private:
    void ThreadLoop(int i);

    static void StopBySignal(int i);

    bool shouldStop;
    bool joined;
    std::mutex mutex;
    std::mutex joinMutex;
    std::condition_variable mutex_condition;
    std::condition_variable joinMutex_condition;
    std::vector<std::thread> threads;
    std::vector<bool> isWorking;
    std::queue<std::function<void()>> jobs;
    static ThreadPool *m_ThreadPool;
    static bool atExitCalled;
};

#endif //THREADPOOL_H
