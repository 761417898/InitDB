#ifndef __RID_H__
#define __RID_H__

class Rid {
    int pageId_;
    int numId_;
public:
    Rid(int pageId, int numId) : pageId_(pageId), numId_(numId) {}
    bool operator==(const Rid& rid) {
        return pageId_ == rid.pageId_ && numId_ == rid.numId_;
    }
    int GetPageId() const {
        return pageId_;
    }
    int GetNumId() const {
        return numId_;
    }
    friend bool operator<(const Rid& rid1, const Rid& rid2) {
        if (rid1.pageId_ < rid2.pageId_) {
            return true;
        } else if (rid1.pageId_ == rid2.pageId_ && rid1.numId_ < rid2.numId_) {
            return true;
        } else {
            return false;
        }
    }
};

#endif