#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>

#include <queue>

namespace ThreadPool{

class ThreadPool {
public:
    using task_t = void (*)(void *);
    using args_t = void *;

    static const int default_core_thread_nums_;
    static const int default_max_thread_nums_;
    static const int default_max_que_size_;

private:
    class Task {
    public:
        Task() = delete;
        Task(task_t, args_t);
        Task(const Task &task) = delete;
        Task(const Task &&task);

        task_t getTask();
        args_t getArgs();

    private:
        task_t task_;
        args_t args_;
    };

public:
    ThreadPool(int=default_max_que_size_);
    ThreadPool(int, int, int=default_max_que_size_);
    virtual ~ThreadPool();

    void shutdownPool();

    void addTask(task_t, args_t);

    int getExistedThreadNums();
    int getBusyThreadNums();
    int getCreatedThreadNums();
    int getExitedThreadNums();

    static void managerThread(ThreadPool *);
    static void workerThread(ThreadPool *);

private:
    bool working_;

    int core_thread_nums_;
    int max_thread_nums_;
    int max_que_size_;

    int existed_thread_nums_;
    int busy_thread_nums_;

    int created_thread_nums_;
    int exited_thread_nums_;

    std::condition_variable notes_;

    std::mutex mutex_pool_;
    std::mutex mutex_que_;

    std::queue<Task> tasks_que_;

    // std::thread manager_;
    // std::vector<std::thread> worker_array_;

    void initThreadPool();
};

} // namespace ThreadPool
#endif