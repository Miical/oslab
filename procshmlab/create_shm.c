/* Env: Macos 13.4 (22F66) */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/semaphore.h>

#define NUM_PASS 10
sem_t *sema1, sema2;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Arguments Error.\n");
        exit(EXIT_FAILURE);
    }

    // Create semaphore used for sync.
    sem_unlink("sema1");
    sem_unlink("sema2");
    if ((sema1 = sem_open("sema1", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
        perror("semaphore error"); exit(EXIT_FAILURE);
    }
    if ((sema1 = sem_open("sema2", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
        perror("semaphore error"); exit(EXIT_FAILURE);
    }
    printf("[CREATE_SHM] Successfully create shared memory '%s'\n", argv[1]);

    // Create shared memory.
    if (shm_unlink(argv[1]) == -1) {
        perror("Can't unlink last shared memory");
        exit(EXIT_FAILURE);
    }
    int fd = shm_open(argv[1], O_CREAT|O_RDWR, 0666);
    if (fd == -1) {
        perror("Can't open shared memory");
        exit(EXIT_FAILURE);
    }

    // Extend shared memroy.
    if (ftruncate(fd, atoi(argv[2])) == -1) {
        perror("ftruncate error");
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1) {
        perror("close error");
        exit(EXIT_FAILURE);
    }
    return 0;
}
