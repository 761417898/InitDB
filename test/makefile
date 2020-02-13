WORKDIR = ..
INCLUDE = $(WORKDIR)/include
TEST = $(WORKDIR)/test
CFLAGS = -g -Wall -I $(INCLUDE) -lgtest -lpthread

ThreadSafeMapTest: $(TEST)/common/ThreadSafeMapTest.cc $(INCLUDE)/common/ThreadSafeMap.h
	g++ -o ThreadSafeMapTest $(TEST)/common/ThreadSafeMapTest.cc $(CFLAGS)

LockManagerTest: $(TEST)/lock/LockManagerTest.cc $(INCLUDE)/common/ThreadSafeMap.h $(WORKDIR)/LockManager.o
	g++ -o LockManagerTest $(TEST)/lock/LockManagerTest.cc $(WORKDIR)/LockManager.o $(CFLAGS)

ThreadSafeQueueTest: $(TEST)/common/ThreadSafeQueueTest.cc $(INCLUDE)/common/ThreadSafeQueue.h
	g++ -o ThreadSafeQueueTest $(TEST)/common/ThreadSafeQueueTest.cc $(CFLAGS)

ThreadPoolTest: $(TEST)/common/ThreadPoolTest.cc $(INCLUDE)/common/ThreadPool.h
	g++ -o ThreadPoolTest $(TEST)/common/ThreadPoolTest.cc $(CFLAGS)

LruReplacerTest: $(TEST)/buffer/LruReplacerTest.cc $(INCLUDE)/buffer/LruReplacer.h
	g++ -o LruReplacerTest $(TEST)/buffer/LruReplacerTest.cc $(CFLAGS)

BufferPoolTest: $(TEST)/buffer/BufferPoolTest.cc $(INCLUDE)/buffer/BufferPool.h $(WORKDIR)/buffer/BufferPool.cc
	g++ -o BufferPoolTest $(WORKDIR)/buffer/BufferPool.cc $(WORKDIR)/disk/DiskManager.cc $(TEST)/buffer/BufferPoolTest.cc $(CFLAGS)

DiskManagerTest: $(TEST)/disk/DiskManagerTest.cc $(INCLUDE)/disk/DiskManager.h $(WORKDIR)/disk/DiskManager.cc
	g++ -o DiskManagerTest $(WORKDIR)/disk/DiskManager.cc $(TEST)/disk/DiskManagerTest.cc $(CFLAGS)

BPlusTreeTest: $(TEST)/common/BPlusTreeTest.cc $(WORKDIR)/common/BPlusTree.cc 
	g++ -o BPlusTreeTest $(WORKDIR)/disk/DiskManager.cc $(TEST)/common/BPlusTreeTest.cc $(WORKDIR)/common/BPlusTree.cc $(WORKDIR)/buffer/BufferPool.cc $(CFLAGS)

clean:
	rm ThreadSafeMapTest LockManagerTest ThreadSafeQueueTest ThreadPoolTest LruReplacerTest BufferPoolTest DiskManagerTest

