TEST = test
INCLUDE = include
CFLAGS = -c -g -Wall -I $(INCLUDE) -std=c++11

LockManager.o: lock/LockManager.cc $(INCLUDE)/lock/LockManager.h $(INCLUDE)/Configure.h $(INCLUDE)/Rid.h $(INCLUDE)/common/ThreadSafeMap.h
	g++ -o LockManager.o lock/LockManager.cc $(CFLAGS)

clean:
	rm LockManager.o
