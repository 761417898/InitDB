#include <gtest/gtest.h>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <chrono>
#include "lock/LockManager.h"
#include "Configure.h"

TEST(LockManagerTest, WaitDieTest) {
    {
        TransactionId transactionId = 100;
        Rid rid(0, 0);
        LockManager lockManager;
        EXPECT_TRUE(lockManager.LockX(transactionId, rid));
    }
    {
        TransactionId transactionId = 100;
        Rid rid(0, 0);
        LockManager lockManager;
        EXPECT_TRUE(lockManager.LockX(transactionId, rid));
        //不满足wait-die
        TransactionId transactionId2 = 101;
        EXPECT_FALSE(lockManager.LockX(transactionId2, rid));
    }
}

TEST(LockManagerTest, LockXTest) {
    {
        TransactionId transactionId = 100;
        Rid rid(0, 0);
        LockManager lockManager;
        EXPECT_TRUE(lockManager.LockX(transactionId, rid));

        lockManager.Unlock(transactionId, rid);

        TransactionId transactionId2 = 99;
        EXPECT_TRUE(lockManager.LockX(transactionId2, rid));
    }
    {
        TransactionId transactionId = 100;
        Rid rid(0, 0);
        LockManager lockManager;
        EXPECT_TRUE(lockManager.LockX(transactionId, rid));

        std::thread testThread ([&lockManager, &rid]() {
            TransactionId transactionId2 = 99;
            std::cout << "wait for LockX\n";
            EXPECT_TRUE(lockManager.LockX(transactionId2, rid));
            std::cout << "LockX success\n";
        });

        std::this_thread::sleep_for(std::chrono::seconds(2));
        lockManager.Unlock(transactionId, rid);
        testThread.join();
    }
}

TEST(LockManagerTest, LockSTest) {
    {
        TransactionId transactionId = 100;
        Rid rid(0, 0);
        LockManager lockManager;
        EXPECT_TRUE(lockManager.LockS(transactionId, rid));

        lockManager.Unlock(transactionId, rid);

        TransactionId transactionId2 = 99;
        EXPECT_TRUE(lockManager.LockS(transactionId2, rid));
    }
    {
        TransactionId transactionId = 100;
        Rid rid(0, 0);
        LockManager lockManager;
        EXPECT_TRUE(lockManager.LockS(transactionId, rid));

        std::thread testThread ([&lockManager, &rid]() {
            TransactionId transactionId2 = 99;
            std::cout << "wait for LockS\n";
            EXPECT_TRUE(lockManager.LockS(transactionId2, rid));
            std::cout << "LockX success\n";
        });

        std::this_thread::sleep_for(std::chrono::seconds(2));
        lockManager.Unlock(transactionId, rid);
        testThread.join();
    }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}