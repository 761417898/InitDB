#ifndef __THREADSAFEQUEUE_H__
#define __THREADSAFEQUEUE_H__

#include <mutex>
#include <condition_variable>

template <typename T>
class ThreadSafeQueue {
    struct Node {
        T data_;
        Node* next_;
        Node() : data_(), next_(nullptr) {}
        Node(T data) : data_(data), next_(nullptr) {}
    };
    Node* head_;
    Node* tail_;
    //可以在const函数中改变值
    mutable std::mutex headMutex_;
    mutable std::mutex tailMutex_;
    std::condition_variable cond;

public:
    ThreadSafeQueue() : head_(new Node()), tail_(head_) {}
    ThreadSafeQueue(const ThreadSafeQueue& threadSafeQueue) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue& threadSafeQueue) = delete;
    ~ThreadSafeQueue() {
        std::lock_guard<std::mutex> lkHeadMutex(headMutex_);
        std::lock_guard<std::mutex> lkTailMutex(tailMutex_);
        auto oldHead = head_;
        while (head_ != tail_) {
            head_ = head_->next_;
            delete oldHead;
            oldHead = head_;
        }
        delete head_;
    }

    T WaitAndPop() {
        //不能调用empty判断，因为可能在 empty返回true 和 真正pop前，别的线程把他pop了
        std::unique_lock<std::mutex> uk(headMutex_);
        /* 相当于。
         *  if (!p) {
         *      cond.wait(uk);
         * }
         */
        cond.wait(uk, [this]() -> bool {
            std::lock_guard<std::mutex> lk(tailMutex_);
            return head_ != tail_;
        });
        auto oldHead = head_;
        head_ = head_->next_;
        auto ret = head_->data_;
        delete oldHead;
        return ret;
    }

    bool TryPop(T &data) {
        std::unique_lock<std::mutex> uk(headMutex_);
        tailMutex_.lock();
        bool empty = head_ == tail_;
        tailMutex_.unlock();
        if (!empty) {
            auto oldHead = head_;
            head_ = head_->next_;
            data = head_->data_;
            delete oldHead;
            return true;
        }
        return false;
    }

    bool Empty() const {
        std::lock_guard<std::mutex> lkHeadMutex(headMutex_);
        std::lock_guard<std::mutex> lkTailMutex(tailMutex_);
        return head_ == tail_;
    }

    void Push(T data) {
        std::lock_guard<std::mutex> lk(tailMutex_);
        tail_->next_ = new Node(data);
        tail_ = tail_->next_;
        cond.notify_one();
    }
};

#endif
