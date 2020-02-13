#include <gtest/gtest.h>
#include <mutex>
#include <thread>
#include <vector>
#include <atomic>
#include "common/BPlusTree.h"

TEST(BPlusTreeTest, InsertTest) {
{
    BufferPool bufferPool;
    BPlusTree<int, int> bPlusTree(&bufferPool);
    int testMaxSize = 508;
    for (int i = 0; i < testMaxSize; ++i) {
        bPlusTree.Insert(i, i);
    }
    //第一次分裂,左边255个元素,右边254
    bPlusTree.Insert(testMaxSize, testMaxSize);
    //再插入254
    for (int i = 1; i <= testMaxSize / 2; ++i) {
        bPlusTree.Insert(i + testMaxSize, i + testMaxSize);
    }
    //测试 非递归插入且不产生新根节点，但叶节点分裂的情况。
    bPlusTree.Insert(testMaxSize / 2 + testMaxSize + 1,
    testMaxSize / 2 + testMaxSize + 1);

    for (int i = 0; i < testMaxSize / 2 + testMaxSize + 1; ++i) {
        int realVal;
        EXPECT_TRUE(bPlusTree.Find(i, realVal));
        EXPECT_EQ(realVal, i);
    }

    auto vec = bPlusTree.Scan(0, testMaxSize / 2 + testMaxSize + 1);
    EXPECT_EQ(vec.size(), testMaxSize / 2 + testMaxSize + 1);
}

{
    BufferPool bufferPool;
    BPlusTree<int, int> bPlusTree(&bufferPool);
    int testMaxSize = 130048;
    for (int i = 0; i < testMaxSize; ++i) {
        bPlusTree.Insert(i, i);

        int realVal;
        EXPECT_TRUE(bPlusTree.Find(i, realVal));
        EXPECT_EQ(realVal, i);
    }
    //递归插入，叶节点溢出，且该叶节点对应父节点也溢出，递归插入，生成了新的根节点
    bPlusTree.Insert(testMaxSize, testMaxSize);
    int realVal;
    EXPECT_TRUE(bPlusTree.Find(testMaxSize, realVal));
    EXPECT_EQ(realVal, testMaxSize);

    bPlusTree.Insert(testMaxSize + 1, testMaxSize + 1);
    EXPECT_TRUE(bPlusTree.Find(testMaxSize + 1, realVal));
    EXPECT_EQ(realVal, testMaxSize + 1);

    auto vec = bPlusTree.Scan(0, testMaxSize + 2);
    EXPECT_EQ(vec.size(), testMaxSize + 2);
}

}

TEST(BPlusTreeTest, IteratorTest) {

}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}