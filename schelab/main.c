#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "sche.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Arguments Error\n");
        exit(EXIT_FAILURE);
    }

    enum SCHE sche;
    if (!strcmp(argv[1], "FCFS")) sche = FCFS;
    else if (!strcmp(argv[1], "SJF")) sche = SJF;
    else if (!strcmp(argv[1], "PSA")) sche = PSA;
    else {
        fprintf(stderr, "Scheduling algorithm %s is not implemented\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    sche_init(sche);

    addJob(10, 1, 8);
    addJob(15, 3, 1);
    addJob(2, 2, 4);
    addJob(6, 9, 12);
    addJob(11, 7, 5);
    addJob(16, 4, 3);
    addJob(13, 1, 2);
    addJob(5, 1, 2);
    addJob(17, 4, 5);
    addJob(4, 4, 3);
    addJob(20, 20, 8);
    addJob(21, 4, 7);

    while (sche_once() != -1);
    summary();

    return 0;
}
