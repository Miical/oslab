#ifndef SCHE_H_
#define SCHE_H_

enum SCHE {
    FCFS, SJF, PSA
};

void sche_init(enum SCHE sche);
bool addJob(int arrivetime, int duration, int priority);
int sche_once();
void summary();

#endif
