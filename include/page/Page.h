#ifndef __PAGE_H__
#define __PAGE_H__

#include "Configure.h"

class Page {
    char data_[PAGESIZE];
    int refCount_;
    bool dirty_;
    PageId pageId_;
public:
    Page() : refCount_(0), dirty_(false), pageId_(INVALIDPAGEID) {}
    char* GetData() {
        return data_;
    }
    PageId GetPageId() const {
        return pageId_;
    }
    void SetDirty() {
        dirty_ = true;
    };
    friend class BufferPool;
};

#endif