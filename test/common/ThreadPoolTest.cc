#include <gtest/gtest.h>
#include <mutex>
#include <thread>
#include <vector>
#include <atomic>
#include "common/ThreadPool.h"

TEST(ThreadPoolTest, BasicTest) {
    std::atomic<int> cnt(0);
    TaskType task([&cnt](int) {
        cnt++;
    }, 0);
    ThreadPool<TaskType> threadPool;
    int expectCnt = 10;
    for (int i = 0; i < expectCnt; ++i) {
        threadPool.Submit(task);
    }
    threadPool.Stop();
    EXPECT_EQ(cnt.load(), expectCnt);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}