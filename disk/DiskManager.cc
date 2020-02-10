#include "disk/DiskManager.h"

void DiskManager::ReadPage(PageId pageId, char* buffer) {
    tableFile_.seekg(pageId * PAGESIZE);
    tableFile_.read(buffer, PAGESIZE);
}

void DiskManager::WritePage(PageId pageId, const char* buffer) {
    tableFile_.seekp(pageId * PAGESIZE);
    tableFile_.write(buffer, PAGESIZE);
}