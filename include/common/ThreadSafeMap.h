#ifndef __THREADSAFEMAP_H__
#define __THREADSAFEMAP_H__

#include <map>
#include <mutex>
#include "Rid.h"

template <typename Key, typename Value>
class ThreadSafeMap {
    std::map<Key, Value> map_;
    std::mutex mtx_;
public:
    //重复插入返回false
    bool Insert(Key key, Value value);
    //查找成功返回true,返回值在value中
    bool Find(const Key& key, Value& value);

    bool Erase(const Key& key);

    void ReleaseMemory();
};

template <typename Key, typename Value>
bool ThreadSafeMap<Key, Value>::Erase(const Key& key) {
    std::lock_guard<std::mutex> lockGuard(mtx_);
    auto iter = map_.find(key);
    if (iter == map_.end()) {
        return false;
    }
    map_.erase(iter);
    return true;
}

template <typename Key, typename Value>
bool ThreadSafeMap<Key, Value>::Find(const Key& key, Value& value) {
    std::lock_guard<std::mutex> lockGuard(mtx_);
    auto iter = map_.find(key);
    if (iter == map_.end()) {
        return false;
    }
    value = iter->second;
    return true;
}

//已存在就false
template <typename Key, typename Value>
bool ThreadSafeMap<Key, Value>::Insert(Key key, Value value) {
    std::lock_guard<std::mutex> lockGuard(mtx_);
    if (map_.find(key) == map_.end()) {
        map_.insert(std::make_pair(key, value));
        return true;
    }
    return false;
}

template <typename Key, typename Value>
void ThreadSafeMap<Key, Value>::ReleaseMemory() {
    std::lock_guard<std::mutex> lockGuard(mtx_);
    for (auto iter = map_.begin(); iter != map_.end(); ++iter) {
        delete iter->second;
    }
}

#endif