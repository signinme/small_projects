#include "ThreadPool.h"
#include <iostream>


namespace ThreadPool {


ThreadPool::Task::Task(task_t task, args_t args) : task_(task), args_(args) {}

ThreadPool::Task::Task(const Task &&task) : task_(task.task_), args_(task.args_) {}

ThreadPool::task_t ThreadPool::Task::getTask() {
    return task_;
}

ThreadPool::args_t ThreadPool::Task::getArgs() {
    return args_;
}

const int ThreadPool::default_core_thread_nums_ = 5;
const int ThreadPool::default_max_thread_nums_ = 10;
const int ThreadPool::default_max_que_size_ = 50;

ThreadPool::ThreadPool(int mq_size) : core_thread_nums_(default_core_thread_nums_), max_thread_nums_(default_max_thread_nums_), max_que_size_(mq_size) {
    initThreadPool();
}

ThreadPool::ThreadPool(int ct_nums, int mt_nums, int mq_size) : core_thread_nums_(ct_nums), max_thread_nums_(mt_nums), max_que_size_(mq_size) {
    initThreadPool();
}

ThreadPool::~ThreadPool() {
    shutdownPool();
}

void ThreadPool::shutdownPool() {
    std::unique_lock<std::mutex> qlck(mutex_que_);
    std::unique_lock<std::mutex> plck(mutex_pool_);
    working_ = false;
    notes_.notify_all();
    qlck.unlock();

    if (existed_thread_nums_ > 0) {
        notes_.wait(plck, [this]() -> bool
                    { return this->existed_thread_nums_ == 0; });
    }
    tasks_que_ = std::queue<Task>();
}

void ThreadPool::initThreadPool() {
    working_ = true;
    existed_thread_nums_ = 0;
    busy_thread_nums_ = 0;

    created_thread_nums_ = 0;
    exited_thread_nums_ = 0;

    worker_array_.resize(max_thread_nums_);

    // manager_ = std::thread(managerThread, this);
}

void ThreadPool::managerThread(ThreadPool *threadpool) {
    ThreadPool &pool = *threadpool;
}

void ThreadPool::workerThread(ThreadPool *threadpool) {
    ThreadPool &pool = *threadpool;

    while (true) {
        std::unique_lock<std::mutex> qlck(pool.mutex_que_);
        if (pool.tasks_que_.empty() && pool.working_) {
            std::unique_lock<std::mutex> plck(pool.mutex_pool_);
            pool.busy_thread_nums_ -= 1;
            if (pool.existed_thread_nums_ > pool.core_thread_nums_) {
                pool.exited_thread_nums_ += 1;
                pool.existed_thread_nums_ -= 1;
                pool.notes_.notify_one();
                return;
            }
            plck.unlock();

            pool.notes_.wait(qlck, [&pool]() -> bool
                             { return pool.tasks_que_.empty() == false || pool.working_ == false; });

            plck.lock();
            pool.busy_thread_nums_ += 1;
        }

        if (pool.working_ == false) {
            std::unique_lock<std::mutex> plck(pool.mutex_pool_);
            pool.exited_thread_nums_ += 1;
            pool.existed_thread_nums_ -= 1;
            pool.notes_.notify_one();
            return;
        }

        task_t task = pool.tasks_que_.front().getTask();
        args_t args = pool.tasks_que_.front().getArgs();
        pool.tasks_que_.pop();
        qlck.unlock();
        
        task(args);
    }
}

void ThreadPool::addTask(task_t task, args_t args) {
    std::unique_lock<std::mutex> qlck(mutex_que_);
    std::unique_lock<std::mutex> plck(mutex_pool_);

    if (existed_thread_nums_ > busy_thread_nums_) {
        tasks_que_.push(Task(task, args));
        notes_.notify_one();
        return;
    }

    if (existed_thread_nums_ < max_thread_nums_) {
        tasks_que_.push(Task(task, args));

        std::thread sub_thread(workerThread, this);
        sub_thread.detach();

        created_thread_nums_ += 1;
        existed_thread_nums_ += 1;
        busy_thread_nums_ += 1;
        notes_.notify_one();
        return;
    }

    if (true || tasks_que_.size() < max_que_size_) {
        tasks_que_.push(Task(task, args));
        notes_.notify_all();
        return;
    }

    throw std::string("too many tasks");
}

int ThreadPool::getBusyThreadNums() {
    std::unique_lock<std::mutex> plck(mutex_pool_);
    int busy_thread_nums = busy_thread_nums_;
    return busy_thread_nums;
}

int ThreadPool::getExistedThreadNums() {
    std::unique_lock<std::mutex> plck(mutex_pool_);
    int existed_thread_nums = existed_thread_nums_;
    return existed_thread_nums;
}

int ThreadPool::getCreatedThreadNums() {
    std::unique_lock<std::mutex> plck(mutex_pool_);
    return created_thread_nums_;
}

int ThreadPool::getExitedThreadNums() {
    std::unique_lock<std::mutex> plck(mutex_pool_);
    return exited_thread_nums_;
}

} // namespace ThreadPool