#include "buffer/BufferPool.h"
#include <gtest/gtest.h>
#include <mutex>
#include <thread>
#include <vector>
#include <atomic>

TEST(BufferPoolTest, NewPageTest) {
    BufferPool bufferPool;
    Page* newPage = bufferPool.NewPage();
    EXPECT_EQ(newPage->GetPageId(), 1);
}

TEST(BufferPoolTest, ReadPageBasicTest) {
    BufferPool bufferPool;
    Page* newPage = bufferPool.NewPage();
    EXPECT_EQ(newPage->GetPageId(), 1);

    char testChar = 'a';
    memset(newPage->GetData(), testChar, PAGESIZE);
    newPage->SetDirty();

    bufferPool.UnpinPage(newPage);
    newPage = bufferPool.ReadBuffer(1);
    for (int i = 0; i < PAGESIZE; ++i) {
        EXPECT_EQ(newPage->GetData()[i], testChar);
    }
}

TEST(BufferPoolTest, PageReplaceTest) {
    BufferPool bufferPool(2);

    Page* page1 = bufferPool.NewPage();
    EXPECT_EQ(page1->GetPageId(), 1);

    Page* page2 = bufferPool.NewPage();
    EXPECT_EQ(page2->GetPageId(), 2);

    Page* page3 = bufferPool.NewPage();
    EXPECT_EQ(page3, nullptr);

    char testChar1 = 'a';
    memset(page1->GetData(), testChar1, PAGESIZE);
    page1->SetDirty();

    char testChar2 = 'b';
    memset(page2->GetData(), testChar2, PAGESIZE);
    page2->SetDirty();

    //释放page1
    bufferPool.UnpinPage(page1);
    bufferPool.UnpinPage(page2);

    page3 = bufferPool.NewPage();
    EXPECT_EQ(page3->GetPageId(), 3);
    char testChar3 = 'c';
    memset(page3->GetData(), testChar3, PAGESIZE);
    page3->SetDirty();

    bufferPool.UnpinPage(page3);

    page1 = bufferPool.ReadBuffer(1);
    for (int i = 0; i < PAGESIZE; ++i) {
        EXPECT_EQ(page1->GetData()[i], testChar1);
    }
    bufferPool.UnpinPage(page1);

    page2 = bufferPool.ReadBuffer(2);
    for (int i = 0; i < PAGESIZE; ++i) {
        EXPECT_EQ(page2->GetData()[i], testChar2);
    }
    bufferPool.UnpinPage(page2);

    page3 = bufferPool.ReadBuffer(3);
    for (int i = 0; i < PAGESIZE; ++i) {
        EXPECT_EQ(page3->GetData()[i], testChar3);
    }
    bufferPool.UnpinPage(page3);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}