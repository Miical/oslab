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
    if (!strcmp(argv[1], "RR")) sche = RR;
    else if (!strcmp(argv[1], "PRIO")) sche = PRIO;
    else {
        fprintf(stderr, "Scheduling algorithm %s is not implemented\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    sche_init(sche);

    add_process(10, 1, 8);
    add_process(15, 3, 1);
    add_process(2, 2, 4);
    add_process(6, 9, 12);
    add_process(11, 7, 5);
    add_process(16, 4, 3);
    add_process(13, 1, 2);
    add_process(5, 1, 2);
    add_process(17, 4, 5);
    add_process(4, 4, 3);
    add_process(20, 20, 8);
    add_process(21, 4, 7);

    while (sche_once() != -1);
    summary();

    return 0;
}
