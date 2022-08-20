#include "threadpool.h"

ThreadPool::ThreadPool() : shouldStop(false), joined(false) {
    const unsigned int NUM_OF_THREADS = std::thread::hardware_concurrency();
    threads.reserve(NUM_OF_THREADS);
    for (unsigned int i = 0; i < NUM_OF_THREADS; i++)
        threads.emplace_back(std::thread(&ThreadPool::ThreadLoop, this));
}

ThreadPool::~ThreadPool() {
    if (joined) {
        threads.clear();
        return;
    }
    {
        std::unique_lock<std::mutex> lock(mutex);
        shouldStop = true;
    }
    mutex_condition.notify_all();
    for (std::thread &thread: threads)
        if (thread.joinable())
            thread.join();
    threads.clear();
}

template<typename Func, typename... Args>
void ThreadPool::QueueJob(Func job, Args&&... args) {
    {
        std::unique_lock<std::mutex> lock(mutex);
        jobs.push(std::bind(job, std::forward<Args>(args)...));
    }
    mutex_condition.notify_one();
}

bool ThreadPool::isBusy() {
    bool isBusy;
    {
        std::unique_lock<std::mutex> lock(mutex);
        isBusy = !jobs.empty();
    }
    return isBusy;
}

void ThreadPool::Join() {
    {
        std::unique_lock<std::mutex> lock(mutex);
        shouldStop = true;
    }
    mutex_condition.notify_all();
    for (std::thread &thread: threads)
        if (thread.joinable())
            thread.join();
    joined = true;
}

void ThreadPool::ThreadLoop() {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(mutex);
            mutex_condition.wait(lock, [this] { return !jobs.empty() || shouldStop; });
            if (shouldStop && jobs.empty())
                return;
            if (!jobs.empty()) {
                job = jobs.front();
                jobs.pop();
            }
        }
        job();
    }
}

ThreadPool *ThreadPool::m_ThreadPool = nullptr;

void ThreadPool::Start() {
    m_ThreadPool = new ThreadPool();
}

void ThreadPool::Stop() {
    delete m_ThreadPool;
    m_ThreadPool = nullptr;
}

void ThreadPool::QueueJob_S(const std::function<void()> &job) {
    if(m_ThreadPool != nullptr)
        m_ThreadPool->QueueJob(job);
}

bool ThreadPool::isBusy_S() {
    if (m_ThreadPool == nullptr)
        return false;
    return m_ThreadPool->isBusy();
}
