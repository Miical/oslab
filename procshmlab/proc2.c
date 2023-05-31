/* Env: Macos 13.4 (22F66) */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/semaphore.h>

#define NUM_PASS 10

sem_t *sema1, *sema2;

int main(int argc, char* argv[]){
    if (argc != 2) {
        printf("Arguments Error.\n");
        exit(EXIT_FAILURE);
    }

    // Get semaphore.
    if ((sema1 = sem_open("sema1", O_EXCL, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
        perror("semaphore error"); exit(EXIT_FAILURE);
    }
    if ((sema2 = sem_open("sema2", O_EXCL, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
        perror("semaphore error"); exit(EXIT_FAILURE);
    }

    // Open and mmap shared memory.
    int fd = shm_open(argv[1], O_RDWR, 0);
    if(fd == -1){
        perror("Can't open shared memory.");
        exit(EXIT_FAILURE);
    }
    void* buff = mmap(NULL, sizeof(int), PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
    if(buff == MAP_FAILED){
        perror("mmap error.");
        exit(EXIT_FAILURE);
    }

    // Read from shared memory.
    sem_wait(sema1);
    printf("[PROC 2] Read numbers: ");
    for (int i = 0; i < NUM_PASS; i++) {
        int num = *((int *)buff + i);
        printf("%d ", num);
    }
    printf("\n");

    // Write to shared memory.
    srand(time(0) * 2);
    printf("\n[PROC 2] Write numbers: ");
    for (int i = 0; i < NUM_PASS; i++) {
        int num = rand() % 100;
        *((int *)buff + i) = num;
        printf("%d ", num);
    }
    printf("\n");
    sem_post(sema2);

    if (munmap(buff, sizeof(int)) == -1) {
        perror("munmap error.");
        exit(EXIT_FAILURE);
    }
    close(fd);
    return 0;
}
