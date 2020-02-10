#ifndef __CONFIGURE_H__
#define __CONFIGURE_H__

#define  TransactionId int
#define PageId int
#define INVALIDPAGEID -1
#define TABLEFILENAME "TableRecord"

enum class LockType {X, S};

const int PAGESIZE = 4096;

#endif