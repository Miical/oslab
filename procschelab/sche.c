#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <limits.h>
#include "sche.h"

#define max(a,b) ((a>b)?(a):(b))
#define min(a,b) ((a<b)?(a):(b))

enum ProcessStatus{
    START, READY, RUNNGING, FINISH
};

struct Process {
    int id;
    int arrivetime;
    int priority;
    int duration;

    int starttime;
    int endtime;
    int timeleft;
    enum ProcessStatus status;
};

// Queue

#define MAXELEMS 128
struct Queue {
    int front, rear;
    int timeslice;
    struct Process* elems[MAXELEMS];
};

static void init_queue(struct Queue* q, int timeslice);
static void push(struct Queue* q, struct Process* proc);
static bool is_empty(struct Queue* q);
static struct Process *pop(struct Queue* q);
static void update_queue(struct Queue* q);

static struct Queue RRqueue;

// Sche

#define MAXPROCESS 128

struct Process* processes[MAXPROCESS];
struct Process* finished[MAXPROCESS];

static int process_cnt;
static int current_time, sche_duration;
static int (*scheduler)(void);

#define RRSLICE 2
static struct Process* latest_process();
static int RR_sche(void);
static int PRIO_sche(void);

/**
 * 调度程序初始化。
 */
void sche_init(enum SCHE sche) {
    process_cnt = 0;
    current_time = 0;
    if (sche == RR) {
        init_queue(&RRqueue, RRSLICE);
        scheduler = RR_sche;
    } else if (sche == PRIO) {
        scheduler = PRIO_sche;
    }
    printf("| Time  | Proc   | PRIO | Arrive Time | Proc Duration "
        "| Sche Time | Time Left |\n");
}

static void printScheMsg(int id) {
    printf("| %02d:%02d | #%-3d   | %2d   | %02d:%02d       "
        "| %4ds         | %4ds     | %4ds     |\n",
        current_time/60, current_time%60,
        id,
        processes[id]->priority,
        processes[id]->arrivetime/60, processes[id]->arrivetime%60,
        processes[id]->duration,
        sche_duration, processes[id]->timeleft);
}

/**
 * 进行一次调度。
 * 在 readylist 中进行一次调度，并更改任务状态。
 * 调度时长保存在 sche_duration 中。
 * 返回调度的任务编号，若没有任何任务被调度则返回 -1。
 */
int sche_once() {
    int id = scheduler();
    if (id != -1) {
        printScheMsg(id);
        current_time += sche_duration;
    }
    return id;
}

/**
 * 创建一个任务，并加入到 readylist中。
 */
bool add_process(int arrivetime, int duration, int priority) {
    if (process_cnt == MAXPROCESS)
        return false;
    struct Process *proc = (struct Process *) malloc(sizeof(struct Process));
    proc->arrivetime = arrivetime;
    proc->duration = duration;
    proc->priority = priority;
    proc->id = process_cnt;
    proc->status = START;
    proc->timeleft = duration;
    processes[process_cnt++] = proc;
    return true;
}

/**
 * 计算调度信息。
 */
void summary() {
    for (int i = 0; i < process_cnt; i++) {
        printf("Process #%-2d: Wait Time: %-3d, Turnaround Time: %-3d\n",
            processes[i]->id,
            processes[i]->starttime - processes[i]->arrivetime,
            processes[i]->endtime - processes[i]->arrivetime
        );
    }
}

static int RR_sche() {
    update_queue(&RRqueue);
    if (is_empty(&RRqueue)) {
        struct Process *latest = latest_process();
        if (latest == NULL) return -1;
        current_time = latest->arrivetime;
        update_queue(&RRqueue);
    }

    struct Process *nxt = pop(&RRqueue);
    sche_duration = min(RRSLICE, nxt->timeleft);

    if (nxt->timeleft == nxt->duration)
        nxt->starttime = current_time;
    nxt->timeleft -= sche_duration;

    if (nxt->timeleft == 0) {
        nxt->status = FINISH;
        nxt->endtime = current_time + sche_duration;
    } else {
        push(&RRqueue, nxt);
    }
    return nxt->id;
}

static int PRIO_sche() {
    struct Process* nxt = NULL;
    for (int i = 0; i < process_cnt; i++) {
        if (processes[i]->status == START && processes[i]->arrivetime <= current_time) {
            processes[i]->status = READY;
        }
    }
    for (int i = 0; i < process_cnt; i++) {
        if (processes[i]->status == READY)
            if (nxt == NULL || processes[i]->priority < nxt->priority)
                nxt = processes[i];
    }
    if (nxt == NULL) {
        nxt = latest_process();
        if (nxt == NULL) return -1;
        current_time = nxt->arrivetime;
    }
    for (int i = 0; i < process_cnt; i++) {
        if (processes[i]->status == START && processes[i]->arrivetime <= current_time) {
            processes[i]->status = READY;
        }
    }

    sche_duration = nxt->timeleft;
    if (nxt->timeleft == nxt->duration)
        nxt->starttime = current_time;
    nxt->timeleft -= sche_duration;

    if (nxt->timeleft == 0) {
        nxt->status = FINISH;
        nxt->endtime = current_time + sche_duration;
    }
    return nxt->id;
}

static struct Process* latest_process() {
    int latest = INT_MAX;
    struct Process *proc = NULL;
    for (int i = 0; i < process_cnt; i++) {
        if (processes[i]->status != FINISH) {
            if (processes[i]->arrivetime < latest) {
                latest = processes[i]->arrivetime;
                proc = processes[i];
            }
        }
    }
    return proc;
}

// Queue

static void init_queue(struct Queue* q, int timeslice) {
    q->front = q->rear = 0;
    q->timeslice = timeslice;
}

static void push(struct Queue* q, struct Process* proc) {
    q->elems[q->rear++] = proc;
}

static bool is_empty(struct Queue* q) {
    return q->front >= q->rear;
}

static struct Process *pop(struct Queue* q) {
    assert(q->rear > q->front);
    return q->elems[q->front++];
}

static void update_queue(struct Queue* q) {
    for (int i = 0; i < process_cnt; i++) {
        if (processes[i]->status == START && processes[i]->arrivetime <= current_time) {
            processes[i]->status = READY;
            push(q, processes[i]);
        }
    }
}
