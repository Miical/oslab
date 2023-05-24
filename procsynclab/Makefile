CC=gcc
CFLAGS=-Wall -g
BINARIES=procsync
OBJS=procsync.o 
tokenizer: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o ${BINARIES}

tokenizer.o: procsync.c
	${CC} -c procsync.c

clean:
	rm -f $(BINARIES) ${OBJS}
