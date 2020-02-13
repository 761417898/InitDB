#ifndef __BPLUSTREE_H__
#define __BPLUSTREE_H__

#include <vector>
#include <iostream>
#include "Configure.h"
#include "page/Page.h"
#include "buffer/BufferPool.h"

enum class PageType {LEAFPAGE, INTERNALPAGE};

template <typename Key, typename Value>
class BPlusTree {
    typedef std::pair<Key, Value> DataItem;
    BufferPool* bufferPool_;
    PageId rootPageId_;

    struct Node {
        PageType pageType_;
        PageId pageId_;
        PageId parentPageId_;
        int size_;
        //最多能放多少个key-value对
        int maxSize_;

        //对于根结点，最后一个dataitem只用value作next指针
        //对于非叶子节点,第一个dataitem只用value作首个指针
        DataItem data_[0];

        Node(PageType pageType, PageId pageId, PageId parentPageId) : pageType_(pageType),
                pageId_(pageId), parentPageId_(parentPageId), size_(0),
                maxSize_((PAGESIZE - sizeof(Node)) / sizeof(DataItem) - 1) {}

        void SetNode(PageType pageType, PageId pageId, PageId parentPageId) {
            pageType_ = pageType;
            pageId_ = pageId;
            parentPageId_ = parentPageId;
            size_ = 0;
            maxSize_ = (PAGESIZE - sizeof(Node)) / sizeof(DataItem);
            data_[GetMaxSize() - 1].second = INVALIDPAGEID;
        }

        PageId GetPageId() const {
            return pageId_;
        }

        int GetSize() const {
            return size_;
        }

        void SetSize(size_t size) {
            size_ = size;
        }

        int GetMaxSize() const {
            return maxSize_;
        }

        bool IsRoot() const {
            return parentPageId_ == INVALIDPAGEID;
        }

        bool IsLeaf() const {
            return pageType_ == PageType::LEAFPAGE;
        }

        void SetNextPageId(PageId pageId) {
            data_[GetMaxSize() - 1].second = pageId;
        }

        PageId GetNextPageId() const {
            return data_[GetMaxSize() - 1].second;
        };
    };

    void InsertInLeaf(Node* leafNode, const Key& key, const Value& value) {
        //本来为空
        if (leafNode->GetSize() == 0) {
            leafNode->data_[0] = std::make_pair(key, value);
            ++leafNode->size_;
            return;
        }

        int idx = 0;
        //找到首个大于key的dataitem
        for (; idx < leafNode->GetSize() &&
                leafNode->data_[idx].first < key; ++idx);

        if (leafNode->data_[idx].first == key) return;;

        for (int i = leafNode->GetSize(); i > idx; --i) {
            leafNode->data_[i] = leafNode->data_[i - 1];
        }

        leafNode->data_[idx] = std::make_pair(key, value);
        ++leafNode->size_;
    }

    Node* GetRootNode() {
        auto rootPage = bufferPool_->ReadBuffer(rootPageId_);
        return reinterpret_cast<Node*>(rootPage->GetData());
    }

    Node* FindLeafNode(const Key& key) {
        Node* node = GetRootNode();
        while (node->pageType_ != PageType::LEAFPAGE) {
            int idx = 0;
            //跳过首个不用的节点
            ++idx;
            if (key < node->data_[idx].first) {
                --idx;
            } else {
                for (; idx < node->GetSize() &&
                       node->data_[idx].first <= key; ++idx);
                --idx;
            }
            PageId nextPageId = node->data_[idx].second;
            //释放本节点，从缓冲区取出下一节点
            bufferPool_->UnpinPage(reinterpret_cast<Page*>(node));
            Page* nextPage = bufferPool_->ReadBuffer(nextPageId);
            node = reinterpret_cast<Node*>(nextPage->GetData());
        }
        return node;
    }

    //要求from to非同属同一节点
    void Copy(DataItem* from, DataItem* to, int fromOff, int toOff, int len) {
        while (len--) {
            to[toOff++] = from[fromOff++];
        }
    }

    void InsertInParent(Node* prevNode, Key key, Node* nextNode) {
        if (prevNode->IsRoot()) {
            auto rootPage = bufferPool_->NewPage();
            assert(rootPage != nullptr);
            auto rootNode = reinterpret_cast<Node*>(rootPage->GetData());

            rootNode->SetNode(PageType::INTERNALPAGE, rootPage->GetPageId(), INVALIDPAGEID);
            rootNode->data_[0].second = prevNode->pageId_;
            rootNode->data_[1] = std::make_pair(key, nextNode->pageId_);
            rootNode->size_ = 2;

            prevNode->parentPageId_ = rootNode->GetPageId();
            nextNode->parentPageId_ = rootNode->GetPageId();

            //更新根结点
            rootPageId_ = rootPage->GetPageId();

            bufferPool_->UnpinPage(reinterpret_cast<Page*>(rootNode));
            bufferPool_->UnpinPage(reinterpret_cast<Page*>(prevNode));
            bufferPool_->UnpinPage(reinterpret_cast<Page*>(nextNode));
        } else {
            //get prevNode's parent node
            auto parentPage = bufferPool_->ReadBuffer(prevNode->parentPageId_);
            auto parentNode = reinterpret_cast<Node*>(parentPage->GetData());
            //父节点不需要分裂
            if (parentNode->GetSize() < parentNode->GetMaxSize()) {
                //找到要插入的位置
                int idx = 0;
                //跳过首个不用的节点
                ++idx;
                if (key < parentNode->data_[idx].first) {
                    --idx;
                } else {
                    for (; idx < parentNode->GetSize() &&
                           parentNode->data_[idx].first <= key; ++idx);
                    if (idx < parentNode->GetSize()) {
                        --idx;
                    }
                }
                //在父节点中插入新节点
                for (int i = parentNode->GetSize(); i > idx; --i) {
                    parentNode->data_[i] = parentNode->data_[i - 1];
                }
                parentNode->data_[idx] = std::make_pair(key, nextNode->pageId_);
                parentNode->size_++;
                //修改子节点的父亲指针
                prevNode->parentPageId_ = parentNode->GetPageId();
                nextNode->parentPageId_ = parentNode->GetPageId();

                bufferPool_->UnpinPage(reinterpret_cast<Page*>(parentNode));
                bufferPool_->UnpinPage(reinterpret_cast<Page*>(prevNode));
                bufferPool_->UnpinPage(reinterpret_cast<Page*>(nextNode));
            } else {  //父亲节点溢出，需要递归调用InsertInParent
                std::cout << "pause" << std::endl;
                //先找到要插入的位置,先在节点外插入好，再复制回节点
                int idx = 1;
                for (; idx < parentNode->GetSize() &&
                       parentNode->data_[idx].first < key; ++idx);

                int tmpVecSize = parentNode->GetSize() + 1;
                auto tmpVec = new DataItem[tmpVecSize];
                Copy(parentNode->data_, tmpVec, 0, 0, idx);
                tmpVec[idx] = std::make_pair(key, nextNode->pageId_);
                Copy(parentNode->data_, tmpVec, idx, idx + 1, parentNode->GetSize() - idx);

                auto newPage = bufferPool_->NewPage();
                auto newNode = reinterpret_cast<Node*>(newPage->GetData());
                newNode->SetNode(PageType::INTERNALPAGE, newPage->GetPageId(), parentNode->parentPageId_);

                //把刚才copy的复制回去,注意修改叶节点
                int mid = (tmpVecSize + 1) / 2;
                Copy(tmpVec, parentNode->data_, 0, 0, mid);
                parentNode->size_ = mid;
                prevNode->parentPageId_ = parentNode->GetPageId();

                Copy(tmpVec, newNode->data_, mid, 0, tmpVecSize - mid);
                newNode->size_ = tmpVecSize - mid;
                nextNode->parentPageId_ = newNode->GetPageId();

                delete []tmpVec;
                bufferPool_->UnpinPage(reinterpret_cast<Page*>(prevNode));
                bufferPool_->UnpinPage(reinterpret_cast<Page*>(nextNode));
                InsertInParent(parentNode, newNode->data_[0].first, newNode);
            }
        }
    }

public:
    BPlusTree(BufferPool* bufferPool) : bufferPool_(bufferPool),
            rootPageId_(INVALIDPAGEID) {}

    void Insert(const Key& key, const Value& value) {
        Node* leafNode;
        if (rootPageId_ == INVALIDPAGEID) {
            auto rootPage = bufferPool_->NewPage();
            leafNode = reinterpret_cast<Node*>(rootPage->GetData());

            leafNode->SetNode(PageType::LEAFPAGE, rootPage->GetPageId(), INVALIDPAGEID);
            //更新根结点
            rootPageId_ = rootPage->GetPageId();
        } else {
            leafNode = FindLeafNode(key);
        }
        if (leafNode->GetSize() < leafNode->GetMaxSize() - 1) {
            InsertInLeaf(leafNode, key, value);
            (reinterpret_cast<Page*>(leafNode))->SetDirty();
            //插入完成，释放叶子页并返回
            bufferPool_->UnpinPage(reinterpret_cast<Page*>(leafNode));
            return;
        }

        //先找到要插入的位置,先在节点外插入好，再复制回节点
        int idx = 0;
        for (; idx < leafNode->GetSize() &&
               leafNode->data_[idx].first < key; ++idx);

        int tmpVecSize = leafNode->GetMaxSize();
        auto tmpVec = new DataItem[tmpVecSize];
        Copy(leafNode->data_, tmpVec, 0, 0, idx);
        tmpVec[idx] = std::make_pair(key, value);
        Copy(leafNode->data_, tmpVec, idx, idx + 1, leafNode->GetSize() - idx);

        auto newPage = bufferPool_->NewPage();
        auto newNode = reinterpret_cast<Node*>(newPage->GetData());
        newNode->SetNode(PageType::LEAFPAGE, newPage->GetPageId(), INVALIDPAGEID);
        //设置next
        newNode->SetNextPageId(leafNode->GetNextPageId());
        leafNode->SetNextPageId(newNode->GetPageId());

        //把刚才copy的复制回去
        int mid = (tmpVecSize + 1) / 2;
        Copy(tmpVec, leafNode->data_, 0, 0, mid);
        leafNode->size_ = mid;

        Copy(tmpVec, newNode->data_, mid, 0, tmpVecSize - mid);
        newNode->size_ = tmpVecSize - mid;

        delete []tmpVec;

        InsertInParent(leafNode, newNode->data_[0].first, newNode);
    }

    bool Find(const Key& key, Value& value) {
        auto leafNode = FindLeafNode(key);
        for (int i = 0; i < leafNode->GetSize(); ++i) {
            if (leafNode->data_[i].first == key) {
                value = leafNode->data_[i].second;
                bufferPool_->UnpinPage(reinterpret_cast<Page*>(leafNode));
                return true;
            }
        }
        bufferPool_->UnpinPage(reinterpret_cast<Page*>(leafNode));
        return false;
    }

    //  start <= key < end
    std::vector<Value> Scan(const Key& start, const Key& end) {
        //find start key所在的page
        Node* node = FindLeafNode(start);

        std::vector<Value> ret;
        while (true) {
            for (int i = 0; i < node->GetSize(); ++i) {
                if (node->data_[i].first >= end) {
                    return ret;
                }
                if (node->data_[i].first >= start) {
                    ret.push_back(node->data_[i].second);
                }
            }
            PageId nextPageId = node->GetNextPageId();
            //释放本节点，从缓冲区取出下一节点
            bufferPool_->UnpinPage(reinterpret_cast<Page*>(node));
            if (nextPageId == INVALIDPAGEID) {
                break;
            }
            Page* nextPage = bufferPool_->ReadBuffer(nextPageId);
            node = reinterpret_cast<Node*>(nextPage->GetData());
        }
        return ret;
    }

};

#endif