CC=gcc
CFLAGS=-Wall -g
BINARIES=create_shm proc1 proc2

all: create_shm proc1 proc2

check: create_shm proc1 proc2
	./create_shm shm 128
	./proc1 shm &
	./proc2 shm &
	@sleep 1

proc1: proc1.c
	${CC} ${CFLAGS} proc1.c -o proc1
	
proc2: proc2.c
	${CC} ${CFLAGS} proc2.c -o proc2

create_shm : create_shm.c
	${CC} ${CFLAGS} create_shm.c -o create_shm

clean:
	rm -f $(BINARIES) 
	rm -rf *.dSYM
