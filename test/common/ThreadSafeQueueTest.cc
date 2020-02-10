#include <gtest/gtest.h>
#include <mutex>
#include <thread>
#include <vector>
#include "common/ThreadSafeQueue.h"

TEST(ThreadSafeQueueTest, SingleThreadTest) {
    ThreadSafeQueue<int> threadSafeQueue;
    //push
    for (int i = 1; i <= 10; ++i) {
        threadSafeQueue.Push(i);
    }
    //trypop
    for (int i = 1; i <= 10; ++i) {
        int realValue;
        EXPECT_TRUE(threadSafeQueue.TryPop(realValue));
        EXPECT_EQ(realValue, i);
    }

    for (int i = 1; i <= 10; ++i) {
        threadSafeQueue.Push(i);
    }
    //wait and pop
    for (int i = 1; i <= 10; ++i) {
        EXPECT_EQ(threadSafeQueue.WaitAndPop(), i);
    }
}

TEST(ThreadSafeQueueTest, MultiThreadTest) {
    ThreadSafeQueue<int> threadSafeQueue;

    {
        //trypop
        std::thread thread([&threadSafeQueue]() {
            for (int i = 1; i <= 10; ++i) {
                int realValue;
                while (!threadSafeQueue.TryPop(realValue)) {
                    std::this_thread::yield();
                }
                EXPECT_EQ(realValue, i);
            }
        });
        //push
        for (int i = 1; i <= 10; ++i) {
            threadSafeQueue.Push(i);
        }
        thread.join();
    }

    {
        //waitpop
        std::thread thread([&threadSafeQueue]() {
            for (int i = 1; i <= 10; ++i) {
                EXPECT_EQ(threadSafeQueue.WaitAndPop(), i);
            }
        });
        //push
        for (int i = 1; i <= 10; ++i) {
            threadSafeQueue.Push(i);
        }
        thread.join();
    }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}