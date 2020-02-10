TEST = test
INCLUDE = include
CFLAGS = -c -g -Wall -I $(INCLUDE) -std=c++11 -lpthread

LockManager.o: lock/LockManager.cc $(INCLUDE)/lock/LockManager.h $(INCLUDE)/Configure.h $(INCLUDE)/Rid.h $(INCLUDE)/common/ThreadSafeMap.h
	g++ -o LockManager.o lock/LockManager.cc $(CFLAGS)

main:
	g++ -o main main.cc net/Server.cc -g -Wall -I $(INCLUDE) -std=c++11 -lpthread

clean:
	rm LockManager.o
