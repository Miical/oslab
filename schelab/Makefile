CC=gcc
CFLAGS=-Wall -g
BINARIES=sche
OBJS=main.o sche.o 
sche: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o ${BINARIES}

sche.o: sche.c sche.h
	${CC} ${CFLAGS} -c sche.c

main.o: main.c
	${CC} ${CFLAGS} -c main.c

clean:
	rm -f $(BINARIES) ${OBJS}
