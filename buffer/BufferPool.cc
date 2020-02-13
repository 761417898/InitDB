#include "buffer/BufferPool.h"

//不成功返回nullptr
Page* BufferPool::ReadBuffer(PageId pageId) {
    std::lock_guard<std::mutex> lockGuard(mtx_);

    Page* ret = nullptr;
    //本来就在缓冲区中
    for (auto iter = lruReplacer_.list_.begin(); iter != lruReplacer_.list_.end();
         ++iter) {
        if (((Page*)(*iter))->GetPageId() == pageId) {
            ret = *iter;
            ret->refCount_++;
            //更新lru链表
            lruReplacer_.Erase(ret);
            lruReplacer_.Insert(ret);
            return ret;
        }
    }

    if (!freeList_.empty()) {
        ret = freeList_.back();
        freeList_.pop_back();
    } else {
        //替换
        for (auto iter = lruReplacer_.list_.begin(); iter != lruReplacer_.list_.end();
             ++iter) {
            if (((Page*)(*iter))->refCount_ == 0) {
                ret = *iter;
                lruReplacer_.Erase(ret);
                break;
            }
        }

        if (ret == nullptr) {
            return nullptr;
        }
    }
    ret->refCount_ = 1;
    ret->pageId_ = pageId;
    ret->dirty_ = false;
    lruReplacer_.Insert(ret);

    diskManager_.ReadPage(pageId, ret->GetData());

    return ret;
}

//不成功返回nullptr
Page* BufferPool::NewPage() {
    std::lock_guard<std::mutex> lockGuard(mtx_);
    Page* ret = nullptr;
    if (!freeList_.empty()) {
        ret = freeList_.back();
        freeList_.pop_back();
    } else {
        for (auto iter = lruReplacer_.list_.begin(); iter != lruReplacer_.list_.end();
             ++iter) {
            if (((Page*)(*iter))->refCount_ == 0) {
                ret = *iter;
                lruReplacer_.Erase(ret);
                break;
            }
        }

        if (ret == nullptr) {
            return nullptr;
        }
    }
    ret->refCount_ = 1;
    ret->dirty_ = false;
    memset(ret->GetData(), 0, PAGESIZE);
    ret->pageId_ = diskManager_.AllocatePageId();

    lruReplacer_.Insert(ret);
    return ret;
}

void BufferPool::UnpinPage(Page* page) {
    std::lock_guard<std::mutex> lockGuard(mtx_);
    page->refCount_--;
    if (page->refCount_ == 0 && page->dirty_) {
        Flush(page);
    }
}

void BufferPool::Flush(Page* page) {
    diskManager_.WritePage(page->pageId_, page->GetData());
}

