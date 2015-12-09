/* 	Author info
 *	athimma Arjun Thimmareddy
 * 
 */
CC     = gcc
CFLAGS = -g

mytest: mytest.o mymutex mycond mybarrier
	$(CC) $(CFLAGS) -o mytest mytest.o mythread.a mymutex.o mycond.o mybarrier.o

mymutex: mymutex.c
	$(CC) $(CFLAGS) -c mymutex.c -w

mycond: mycond.c
	$(CC) $(CFLAGS) -c mycond.c -w

mybarrier: mybarrier.c
	$(CC) $(CFLAGS) -c mybarrier.c -w

mytest.o: mytest.c
	$(CC) $(CFLAGS) -c mytest.c -w

clean:
	rm -rf *.o mytest
