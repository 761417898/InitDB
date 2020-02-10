#include <gtest/gtest.h>
#include <mutex>
#include <thread>
#include <vector>
#include <atomic>
#include "disk/DiskManager.h"

TEST(DiskManagerTest, BasicTest) {
    char inputData1[PAGESIZE];
    char inputData2[PAGESIZE];
    int inputPageId1 = 0;
    int inputPageId2 = 1;
    for (int i = 0; i < PAGESIZE; ++i) {
        inputData1[i] = '1';
        inputData2[i] = '2';
    }
    DiskManager diskManager;
    diskManager.WritePage(inputPageId1, inputData1);
    diskManager.WritePage(inputPageId2, inputData2);

    char outData1[PAGESIZE];
    char outData2[PAGESIZE];
    diskManager.ReadPage(inputPageId1, outData1);
    diskManager.ReadPage(inputPageId2, outData2);
    for (int i = 0; i < PAGESIZE; ++i) {
        EXPECT_EQ(inputData1[i], outData1[i]);
        EXPECT_EQ(inputData2[i], outData2[i]);
    }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}