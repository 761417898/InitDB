#include <gtest/gtest.h>
#include <mutex>
#include <thread>
#include <vector>
#include "common/ThreadSafeMap.h"

TEST(ThreadSafeMapTest, SingleThreadTest) {
    ThreadSafeMap<int, int> threadSafeMap;
    //写入
    for (int i = 1; i <= 10; ++i) {
        threadSafeMap.Insert(i, i * 10);
    }
    //读取
    for (int i = 1; i <= 10; ++i) {
        int realValue;
        EXPECT_TRUE(threadSafeMap.Find(i, realValue));
        EXPECT_EQ(realValue, i * 10);
    }
    //插入后删除
    for (int i = 1; i <= 10; ++i) {
        threadSafeMap.Erase(i);
        int realValue;
        EXPECT_FALSE(threadSafeMap.Find(i, realValue));
    }
}

TEST(ThreadSafeMapTest, MultiThreadTest) {
    int nThreads = 4;
    std::vector<std::thread> threads(4);
    ThreadSafeMap<int, int> threadSafeMap;
    //并发写入
    for (int i = 0; i < nThreads; ++i) {
        threads[i] = std::thread([&threadSafeMap, i]() {
            threadSafeMap.Insert(i, i * 10);
        });
    }
    for (int i = 0; i < nThreads; ++i) {
        threads[i].join();
    }
    //并发读取
    for (int i = 0; i < nThreads; ++i) {
        threads[i] = std::thread([&threadSafeMap, i]() {
            int realValue;
            EXPECT_TRUE(threadSafeMap.Find(i, realValue));
            EXPECT_EQ(realValue, i * 10);
        });
    }
    for (int i = 0; i < nThreads; ++i) {
        threads[i].join();
    }
    //并发删除
    for (int i = 0; i < nThreads; ++i) {
        threads[i] = std::thread([&threadSafeMap, i]() {
            threadSafeMap.Erase(i);
            int realValue;
            EXPECT_FALSE(threadSafeMap.Find(i, realValue));
        });
    }
    for (int i = 0; i < nThreads; ++i) {
        threads[i].join();
    }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}