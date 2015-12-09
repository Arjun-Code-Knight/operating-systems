CC=gcc
CFLAGS=-g 

all: clean p4.o p4

p4: 
	$(CC) -lpthread -o p4 p4.o

p4.o:
	$(CC) -lpthread -c p4.c

clean:
	rm -f p4 p4.o endpoints.txt list*.txt
