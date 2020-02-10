#ifndef __DISKMANAGER_H__
#define __DISKMANAGER_H__

#include <fstream>
#include <iostream>
#include "Configure.h"

class DiskManager {
public:
    DiskManager() : nextPageId_(0) {
        tableFile_.open(TABLEFILENAME, std::ios::in | std::ios::out | std::ios::trunc);
    }
    ~DiskManager() {
        tableFile_.close();
    }

    void ReadPage(PageId pageId, char* buffer);

    PageId AllocatePageId() {
        return ++nextPageId_;
    }

    void WritePage(PageId pageId, const char* buffer);

private:
    std::fstream tableFile_;
    PageId nextPageId_;
};

#endif