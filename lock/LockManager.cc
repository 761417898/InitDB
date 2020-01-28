#include "lock/LockManager.h"

bool LockManager::LockX(const TransactionId& transactionId, const Rid& rid) {
    DataItem *dataItem;
    bool found = lockTable_.Find(rid, dataItem);
    if (!found) {
        //首次要求锁定该数据项的事务，必定可以满足
        dataItem = new DataItem(LockType::X);
        if (lockTable_.Insert(rid, dataItem) == false) {
            //已经存在,delete刚刚申请的
            delete dataItem;
            lockTable_.Find(rid, dataItem);
        }
    }

    std::unique_lock<std::mutex> lock(dataItem->mtx_);
    //不满足wait-die死锁预防
    if (dataItem->list_.size() != 0) {
        for (auto iter = dataItem->list_.begin(); iter != dataItem->list_.end(); ++iter) {
            if (transactionId > iter->transactionId_) {
                return false;
            }
        }
    }

    dataItem->list_.push_back(ListNode(transactionId, false, LockType::X));

    dataItem->cv_.wait(lock, [&]() -> bool {
        //在本事务之前不允许有锁
        auto iter = dataItem->list_.begin();
        bool flag = true;
        for (; iter->transactionId_ != transactionId; ++iter) {
            if (iter->granted_) {
                flag = false;
            }
        }
        iter->granted_ = flag;
        return flag;
    });

    return true;
}

bool LockManager::LockS(const TransactionId& transactionId, const Rid& rid) {
    DataItem *dataItem;
    bool found = lockTable_.Find(rid, dataItem);
    if (!found) {
        //首次要求锁定该数据项的事务，必定可以满足
        dataItem = new DataItem(LockType::S);
        if (lockTable_.Insert(rid, dataItem) == false) {
            //已经存在,delete刚刚申请的
            delete dataItem;
            lockTable_.Find(rid, dataItem);
        }
    }

    std::unique_lock<std::mutex> lock(dataItem->mtx_);
    //不满足wait-die死锁预防
    if (dataItem->list_.size() != 0) {
        for (auto iter = dataItem->list_.begin(); iter != dataItem->list_.end(); ++iter) {
            if (transactionId > iter->transactionId_) {
                return false;
            }
        }
    }

    dataItem->list_.push_back(ListNode(transactionId, false, LockType::S));

    dataItem->cv_.wait(lock, [&]() -> bool {
        //在本事务之前不允许有X锁，否则可以授予本事务S锁
        auto iter = dataItem->list_.begin();
        bool flag = true;
        for (; iter->transactionId_ != transactionId; ++iter) {
            if (iter->lockType_ == LockType::X) {
                flag = false;
            }
        }
        iter->granted_ = flag;
        return flag;
    });

    return true;
}

void LockManager::Unlock(const TransactionId& transactionId, const Rid& rid) {
    DataItem *dataItem;
    lockTable_.Find(rid, dataItem);

    std::unique_lock<std::mutex> lock(dataItem->mtx_);

    auto iter = dataItem->list_.begin();
    for (; iter->transactionId_ != transactionId; ++iter) {}

    dataItem->list_.erase(iter);
    //如果没有剩余等待上锁的事务，则在locktable删除本dataItem
    if (dataItem->list_.size() == 0) {
        delete dataItem;
        lockTable_.Erase(rid);
    } else {
        dataItem->cv_.notify_all();
    }
}

LockManager::~LockManager() {
    //realease memory in map;
   lockTable_.ReleaseMemory();
}
