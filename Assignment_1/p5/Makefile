#CXX        := g++34
#CC         := gcc34
#LINK       := g++34 -fPIC

COMPILEFALGS = -Wall -Werror -I. -g


default : clean_a5 lib_a5 test_a5
a5: clean_a5 lib_a5 test_a5

TESTOBJ_A5 = p5.o	
SOURCE_A5 = mythread_attr_stack.c mythread_attr.c p5.c mythread_create.c mythread_idle.c mythread_join.c mythread_queue.c mythread_exit.c mythread_self.c mythread_yield.c mythread_wrapper.c futex.c
HEADERS_A5 = mythread.h mythread_queue.h futex_inline.h myatomic.h futex.h 
OBJS_A5 = $(SOURCE_A5:.c=.o)
LIB_A5 = mythread.a

lib_a5: $(LIB_A5)

mythread.a: $(OBJS_A5) $(HEADERS_A5) 
				ar rcs $(LIB_A5) $(OBJS_A5)

%.o: %.c $(HEADERS_A5) 
	$(CC) $(COMPILEFALGS) -c $< -o $@

clean_a5:
	rm -f $(OBJS_A5) $(LIB_A5) *~ p5

clean:
	rm -f $(OBJS_A5) $(LIB_A5) *~ p5
	
test_a5: 	
	$(CC) -o p5 $(COMPILEFALGS) $(TESTOBJ_A5) $(LIB_A5)
