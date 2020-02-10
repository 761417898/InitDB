#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <atomic>
#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include "ThreadSafeQueue.h"

struct TaskType {
    std::function<void(int)> task_;
    int arg_;
    TaskType(std::function<void(int)> task, int arg) :
            task_(task), arg_(arg) {}
    TaskType() {}
    TaskType(const TaskType& taskType) :
            task_(taskType.task_), arg_(taskType.arg_) {}
    TaskType& operator=(const TaskType& taskType) {
        task_ = taskType.task_;
        arg_ = taskType.arg_;
        return *this;
    }
};

template <typename T>
class ThreadPool {
private:
    ThreadSafeQueue<T> queue_;
    std::atomic<bool> done_;
    int threadNum_;
    std::vector<std::thread> threads;
public:
    ThreadPool(int threadNum = 4) : threadNum_(threadNum) {
        done_.store(false);
        for (int i = 0; i < threadNum_; ++i) {
            threads.push_back(std::thread([this]() {
                while (!done_.load()) {
                    T task;
                    if (queue_.TryPop(task)) {
                        task.task_(task.arg_);
                    }
                }
            }));
        }
    }
    ThreadPool(const ThreadPool& threadPool) = delete;
    ThreadPool& operator=(const ThreadPool& threadPool) = delete;

    //确定主线程不会再提交任务才stop
    void Stop() {
        //等待任务执行完
        while (!queue_.Empty()) {
            std::this_thread::yield();
        }
        done_.store(true);
        for (int i = 0; i < threadNum_; ++i) {
            threads[i].join();
        }
    }

    ~ThreadPool() {}

    void Submit(T func) {
        queue_.Push(func);
    }
};

#endif