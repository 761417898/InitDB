#ifndef __RESPONSE_H__
#define __RESPONSE_H__
/*
 *     data   ----->   char*
 *     |-- 1Bytes --|--1Bytes--|-------|--2Bytes----|------|
 *     |request type|key length|  key  |value length| value|
 *      request type 1:insert 2:delete 3:read 4:write
 */

class Request {

public:
    Request(char* data) {

    }
};

#endif