CC = gcc
CFLAGS = -Wall -g
OBJS = main.o move.o

a.out:	$(OBJS)
	$(CC) $(OBJS)

clean:
	rm -f *~ *.o a.out