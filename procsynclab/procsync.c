/* Env: Macos 13.4 (22F66) */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/_pthread/_pthread_t.h>
#include <semaphore.h>
#include <sys/fcntl.h>
#include <sys/semaphore.h>

#define PRODUCER_NUM 3
#define CONSUMER_NUM 3
#define MAX_ITEMS 5
#define ITEMS_UNIT 18

typedef struct {
    int buf[MAX_ITEMS];
    int item_cnt;
    int front, rear;
    sem_t *mutex, *slots, *items;
} itemslist_t;

itemslist_t items_list;

int producer_items[PRODUCER_NUM][ITEMS_UNIT], producer_num[PRODUCER_NUM];
int consumer_items[CONSUMER_NUM][ITEMS_UNIT], consumer_num[CONSUMER_NUM];

void *producer_func(void *arg) {
    uint64_t producer_id = (uint64_t) arg;
    for (int i = 0; i < ITEMS_UNIT; i++) {
        if (sem_wait(items_list.slots)) exit(EXIT_FAILURE);
        if (sem_wait(items_list.mutex)) exit(EXIT_FAILURE);

        items_list.buf[(++items_list.rear) % MAX_ITEMS] = ++items_list.item_cnt;
        producer_items[producer_id][producer_num[producer_id]++] = items_list.item_cnt;
        printf("Producer %lld put item %d\n", producer_id, items_list.item_cnt);

        if (sem_post(items_list.mutex)) exit(EXIT_FAILURE);
        if (sem_post(items_list.items)) exit(EXIT_FAILURE);
    }
    return NULL;
}

void *consumer_func(void *arg) {
    uint64_t consumer_id = (uint64_t) arg;
    for (int i = 0; i < ITEMS_UNIT; i++) {
        if (sem_wait(items_list.items)) exit(EXIT_FAILURE);
        if (sem_wait(items_list.mutex)) exit(EXIT_FAILURE);

        int item = items_list.buf[(++items_list.front) % MAX_ITEMS];
        consumer_items[consumer_id][consumer_num[consumer_id]++] = item;
        printf("Consumer %lld get item %d\n", consumer_id, item);

        if (sem_post(items_list.mutex)) exit(EXIT_FAILURE);
        if (sem_post(items_list.slots)) exit(EXIT_FAILURE);
    }
    return NULL;
}

int main() {
    pthread_t consumer[CONSUMER_NUM];
    pthread_t producer[PRODUCER_NUM];

    // Initialize semaphore.
    sem_unlink("slots");
    sem_unlink("mutex");
    sem_unlink("items");
    if ((items_list.slots = sem_open("slots", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, MAX_ITEMS)) == SEM_FAILED) {
        fprintf(stderr, "Semaphore Error.\n"); exit(EXIT_FAILURE);
    }
    if ((items_list.mutex = sem_open("mutex", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
        fprintf(stderr, "Semaphore Error.\n"); exit(EXIT_FAILURE);
    }
    if ((items_list.items = sem_open("items", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
        fprintf(stderr, "Semaphore Error.\n"); exit(EXIT_FAILURE);
    }

    // Create producer and consumer threads.
    for (int64_t i = 0; i < PRODUCER_NUM; i++) {
        if (pthread_create(&producer[i], NULL, producer_func, (void *) i)) {
            fprintf(stderr, "can't create thread.\n");
            exit(EXIT_FAILURE);
        }
    }
    for (int64_t i = 0; i < CONSUMER_NUM; i++) {
        if (pthread_create(&consumer[i], NULL, consumer_func, (void *) i)) {
            fprintf(stderr, "can't create thread.\n");
            exit(EXIT_FAILURE);
        }
    }

    // Join threads.
    for (int64_t i = 0; i < PRODUCER_NUM; i++) {
        if (pthread_join(producer[i], NULL)) {
            fprintf(stderr, "can't join producer thread %lld.\n", i);
            exit(EXIT_FAILURE);
        }
    }
    for (int64_t i = 0; i < CONSUMER_NUM; i++) {
        if (pthread_join(consumer[i], NULL)) {
            fprintf(stderr, "can't join consumer thread %lld.\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // Print summary.
    printf("<summary>\n");
    printf("<producer>\n");
    for (int i = 0; i < PRODUCER_NUM; i++) {
        printf("Producer %d put %d items: ", i, producer_num[i]);
        for (int j = 0; j < producer_num[i]; j++)
            printf("%d ", producer_items[i][j]);
        printf("\n");
    }
    printf("<consumer>\n");
    for (int i = 0; i < CONSUMER_NUM; i++) {
        printf("Consumer %d get %d items: ", i, consumer_num[i]);
        for (int j = 0; j < consumer_num[i]; j++)
            printf("%d ", consumer_items[i][j]);
        printf("\n");
    }

    sem_close(items_list.slots);
    sem_close(items_list.mutex);
    sem_close(items_list.items);
    sem_unlink("slots");
    sem_unlink("mutex");
    sem_unlink("items");
    return 0;
}
