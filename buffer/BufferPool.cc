#include "buffer/BufferPool.h"

//不成功返回nullptr
Page* BufferPool::ReadBuffer(PageId pageId) {
    std::lock_guard<std::mutex> lockGuard(mtx_);
    Page* ret;
    //hash succeed
    if (threadSafeMap_.Find(pageId, ret)) {
        //need update
        if (ret->pageId_ != pageId) {
            threadSafeMap_.Erase(pageId);
        } else {
            ret->refCount_++;
            return ret;
        }
    }

    if (!freeList_.empty()) {
        ret = freeList_.back();
        freeList_.pop_back();
    } else {
        lruReplacer_.Victim(ret);
        if (ret->refCount_ > 0) {
            return nullptr;
        }
        lruReplacer_.Erase(ret);
    }
    ret->refCount_ = 1;
    ret->pageId_ =pageId;
    threadSafeMap_.Insert(ret->pageId_, ret);
    lruReplacer_.Insert(ret);

    diskManager_.ReadPage(pageId, ret->GetData());

    return ret;
}

//不成功返回nullptr
Page* BufferPool::NewPage() {
    std::lock_guard<std::mutex> lockGuard(mtx_);
    Page* ret;
    if (!freeList_.empty()) {
        ret = freeList_.back();
        freeList_.pop_back();
    } else {
        lruReplacer_.Victim(ret);
        if (ret->refCount_ > 0) {
            return nullptr;
        }
        lruReplacer_.Erase(ret);
    }
    ret->refCount_ = 1;
    memset(ret->GetData(), 0, PAGESIZE);
    ret->pageId_ = diskManager_.AllocatePageId();
    threadSafeMap_.Insert(ret->pageId_, ret);
    lruReplacer_.Insert(ret);

    return ret;
}

void BufferPool::UnpinPage(Page* page) {
    std::lock_guard<std::mutex> lockGuard(mtx_);
    page->refCount_--;
    if (page->refCount_ == 0 && page->dirty_) {
        Flush(page);
    }
    //未被引用的被优先置换出
    lruReplacer_.Erase(page);
    lruReplacer_.PushBack(page);

    threadSafeMap_.Erase(page->GetPageId());
}

void BufferPool::Flush(Page* page) {
    diskManager_.WritePage(page->pageId_, page->GetData());
}

