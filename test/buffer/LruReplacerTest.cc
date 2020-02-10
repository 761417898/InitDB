#include <gtest/gtest.h>
#include <mutex>
#include <thread>
#include <vector>
#include <atomic>
#include "buffer/LruReplacer.h"

TEST(LruReplacerTest, ConcurencyTest) {
    int testNum = 100;
    LruReplacer<int> replacer;

    {
        std::thread th1([testNum, &replacer]() {
            for (int i = 0; i < testNum / 2; ++i) {
                replacer.Insert(i);
            }
        });
        std::thread th2([testNum, &replacer]() {
            for (int i = testNum / 2; i < testNum; ++i) {
                replacer.Insert(i);
            }
        });
        th1.join();
        th2.join();
    }
    EXPECT_EQ(replacer.Size(), testNum);

    {
        std::thread th1([testNum, &replacer]() {
            for (int i = 0; i < testNum / 2; ++i) {
                replacer.Erase(i);
            }
        });
        std::thread th2([testNum, &replacer]() {
            for (int i = testNum / 2; i < testNum; ++i) {
                replacer.Erase(i);
            }
        });
        th1.join();
        th2.join();
    }
    EXPECT_EQ(replacer.Size(), 0);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}