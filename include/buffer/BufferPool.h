#ifndef __BUFFERPOOL_H__
#define __BUFFERPOOL_H__

#include <mutex>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <list>


#include "LruReplacer.h"
#include "page/Page.h"
#include "Configure.h"
#include "disk/DiskManager.h"
#include <common/ThreadSafeMap.h>

class BufferPool {
    LruReplacer<Page*> lruReplacer_;
    Page* pages_;
    std::list<Page*> freeList_;
    std::mutex mtx_;
    DiskManager diskManager_;
public:
    BufferPool(int pageCount = 10) : pages_(new Page[pageCount]) {
        for (int i = 0; i < pageCount; ++i) {
            freeList_.push_back(&pages_[i]);
        }
    }
    ~BufferPool() {
        delete []pages_;
    }
    Page* ReadBuffer(PageId pageId);

    Page* NewPage();

    void UnpinPage(Page* page);

    void PrintStatus() {
        for (auto iter = lruReplacer_.list_.begin(); iter != lruReplacer_.list_.end();
             ++iter) {
            if (((Page*)(*iter))->refCount_ > 0) {
                auto ret = *iter;
                std::cout << ret << "  refCount: " << ret->refCount_ << std::endl;
            }
        }
    }

private:
    void Flush(Page* page);
};

#endif