#ifndef __LRUREPLACER_H__
#define __LRUREPLACER_H__

#include <list>
#include <mutex>

template <typename T>
class LruReplacer {
private:
    std::list<T> list_;
    mutable std::mutex mtx_;
public:
    void Insert(T data) {
        std::lock_guard<std::mutex> lockGuard(mtx_);
        list_.push_front(data);
    }
    void PushBack(T data) {
        std::lock_guard<std::mutex> lockGuard(mtx_);
        list_.push_back(data);
    }
    bool Erase(const T& data) {
        std::lock_guard<std::mutex> lockGuard(mtx_);
        for (auto iter = list_.begin(); iter != list_.end(); ++iter) {
            if (*iter == data) {
                list_.erase(iter);
                return true;
            }
        }
        return false;
    }
    //返回最旧的值
    void Victim(T& data) {
        std::lock_guard<std::mutex> lockGuard(mtx_);
        for (auto iter = list_.begin(); iter != list_.end(); ++iter) {
            data = *iter;
        }
    }
    int Size() const {
        std::lock_guard<std::mutex> lockGuard(mtx_);
        return list_.size();
    }
    bool Empty() const {
        std::lock_guard<std::mutex> lockGuard(mtx_);
        return list_.size() == 0;
    }
    friend class BufferPool;
};

#endif