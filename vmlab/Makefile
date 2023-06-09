CC = gcc
CFLAGS = -g -O2

OBJS = block.o clock.o main.o

main: $(OBJS)
	$(CC) $(CFLAGS) -o main $(OBJS)

block.o: block.h block.c
clock.o: clock.h clock.c block.h
main.o: clock.h main.c

clean:
	rm -f *~ *.o main
