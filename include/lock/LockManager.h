#include <iostream>
#include <mutex>
#include <list>
#include <condition_variable>
#include "Configure.h"
#include "common/ThreadSafeMap.h"
#include "Rid.h"

class LockManager {
    struct ListNode {
        bool granted_;
        TransactionId transactionId_;
        LockType lockType_;

        ListNode() {}
        ListNode(TransactionId transactionId, bool granted, LockType lockType) :
                granted_(granted), transactionId_(transactionId),
                lockType_(lockType) {}
    };

    struct DataItem {
        std::list<ListNode> list_;
        LockType lockType_;
        std::mutex mtx_;
        std::condition_variable cv_;

        DataItem() {}
        DataItem(LockType lockType) :
                lockType_(lockType) {}
    };

    ThreadSafeMap<Rid, DataItem*> lockTable_;
public:
    bool LockX(const TransactionId& transactionId, const Rid& rid);
    bool LockS(const TransactionId& transactionId, const Rid& rid);
    void Unlock(const TransactionId& transactionId, const Rid& rid);

    LockManager() {}
    LockManager& operator=(const LockManager&) = delete;
    LockManager(const LockManager&) = delete;
    ~LockManager();
};