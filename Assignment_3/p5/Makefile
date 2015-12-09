CC     = gcc
CFLAGS = -g 

mytest: mytest.o myqueue mysched
	$(CC) $(CFLAGS) -o mytest mytest.o mythread.a myqueue.o mysched.o
	
myqueue: myqueue.c
	$(CC) $(CFLAGS) -c myqueue.c -w
	
mysched: mysched.c
	$(CC) $(CFLAGS) -c mysched.c -w -o mysched.o

mytest.o: mytest.c
	$(CC) $(CFLAGS) -c mytest.c -w

clean:
	rm -rf *.o mytest
