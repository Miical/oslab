#ifndef SCHE_H_
#define SCHE_H_

enum SCHE {
    RR, PRIO
};

void sche_init(enum SCHE sche);
bool add_process(int arrivetime, int duration, int priority);
int sche_once();
void summary();

#endif
