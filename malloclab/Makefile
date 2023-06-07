CC = gcc
CFLAGS = -g -O2 -arch x86_64 -m32 -DDEBUG

OBJS = main.o mm.o memlib.o 

main: $(OBJS)
	$(CC) $(CFLAGS) -o main $(OBJS)

main.o: main.c mm.h memlib.h
memlib.o: memlib.c memlib.h
mm.o: mm.c mm.h memlib.h

clean:
	rm -f *~ *.o main
