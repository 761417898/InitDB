WORKDIR = ..
INCLUDE = $(WORKDIR)/include
TEST = $(WORKDIR)/test
CFLAGS = -g -Wall -I $(INCLUDE) -lgtest -lpthread

ThreadSafeMapTest: $(TEST)/common/ThreadSafeMapTest.cc $(INCLUDE)/common/ThreadSafeMap.h
	g++ -o ThreadSafeMapTest $(TEST)/common/ThreadSafeMapTest.cc $(CFLAGS)

LockManagerTest: $(TEST)/lock/LockManagerTest.cc $(INCLUDE)/common/ThreadSafeMap.h $(WORKDIR)/LockManager.o
	g++ -o LockManagerTest $(TEST)/lock/LockManagerTest.cc $(WORKDIR)/LockManager.o $(CFLAGS)


clean:
	rm ThreadSafeMapTest LockManagerTest

