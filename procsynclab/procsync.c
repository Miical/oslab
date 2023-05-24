/* Env: Macos 13.4 (22F66) */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/_pthread/_pthread_t.h>
#include <semaphore.h>
#include <sys/semaphore.h>

#define PRODUCER_NUM 5
#define CONSUMER_NUM 5
#define MAX_ITEMS 100
#define ITEMS_UNIT 20

sem_t item_lock;
int items_list[MAX_ITEMS], list_num, item_cnt;

int producer_items[PRODUCER_NUM][ITEMS_UNIT], producer_num[PRODUCER_NUM];
int consumer_items[CONSUMER_NUM][ITEMS_UNIT], consumer_num[CONSUMER_NUM];

void *producer_func(void *arg) {
    uint64_t producer_id = (uint64_t) arg;
    for (int i = 0; i < ITEMS_UNIT; i++) {
        if (!sem_wait(&item_lock)) exit(EXIT_FAILURE);
        if (list_num != MAX_ITEMS) {
            items_list[list_num++] = ++item_cnt;
            producer_items[producer_id][producer_num[producer_id]++] = item_cnt;
            printf("Producer %lld put item %d\n", producer_id, item_cnt);
        }
        if (!sem_post(&item_lock)) exit(EXIT_FAILURE);
    }
    return NULL;
}

void *consumer_func(void *arg) {
    uint64_t consumer_id = (uint64_t) arg;
    for (int i = 0; i < ITEMS_UNIT; i++) {
        if (!sem_wait(&item_lock)) exit(EXIT_FAILURE);
        if (list_num != 0) {
            consumer_items[consumer_id][consumer_num[consumer_id]++] = items_list[--list_num];
            printf("Consumer %lld get item %d\n", consumer_id, consumer_items[consumer_id][i]);
        }
        if (!sem_post(&item_lock)) exit(EXIT_FAILURE);
    }
    return NULL;
}

int main() {
    pthread_t consumer[CONSUMER_NUM];
    pthread_t producer[PRODUCER_NUM];

    // Initialize semaphore.
    sem_open("item_lock", O_CREAT|O_EXCL, S_IRWXU, 0);

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
        printf("Producer put %d items: ", producer_num[i]);
        for (int j = 0; j < producer_num[i]; j++)
            printf("%d ", producer_items[i][j]);
        printf("\n");
    }
    printf("<consumer>\n");
    for (int i = 0; i < CONSUMER_NUM; i++) {
        printf("Consumer get %d items: ", consumer_num[i]);
        for (int j = 0; j < consumer_num[i]; j++)
            printf("%d ", consumer_items[i][j]);
        printf("\n");
    }

    sem_unlink("item_lock");
    return 0;
}
