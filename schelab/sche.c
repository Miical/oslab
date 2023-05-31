#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include "sche.h"

#define max(a,b) ((a>b)?(a):(b))
#define min(a,b) ((a<b)?(a):(b))

static int FCFS_sche(void);
static int SJF_sche(void);
static int PSA_sche(void);

enum JobStatus{
    READY, RUNNGING, FINISH
};

struct Job {
    int id;
    int priority;
    int arrivetime;
    int duration;

    int starttime;
    int endtime;
    int timeleft;
    enum JobStatus status;
};

#define MAXJOB 128

static int sche_duration;
struct Job readylist[MAXJOB];

static int job_cnt;
static int current_time;
static int (*scheduler)(void);

/**
 * 调度程序初始化。
 */
void sche_init(enum SCHE sche) {
    job_cnt = 0;
    current_time = 0;
    sche_duration = -1;
    switch (sche) {
    case FCFS:
        scheduler = FCFS_sche;
        break;
    case SJF:
        scheduler = SJF_sche;
        break;
    case PSA:
        scheduler = PSA_sche;
        break;
    }
    printf("| Time  | Job ID | PRIO | Arrive Time | Job Duration "
        "| Sche Time | Time Left |\n");
}

static void printScheMsg(int id) {
    printf("| %02d:%02d | #%-3d   | %2d   | %02d:%02d       "
        "| %4ds        | %4ds     | %4ds     |\n",
        current_time/60, current_time%60,
        id,
        readylist[id].priority,
        readylist[id].arrivetime/60, readylist[id].arrivetime%60,
        readylist[id].duration,
        sche_duration, readylist[id].timeleft);
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
bool addJob(int arrivetime, int duration, int priority) {
    if (job_cnt == MAXJOB)
        return false;
    readylist[job_cnt].id = job_cnt;
    readylist[job_cnt].arrivetime = arrivetime;
    readylist[job_cnt].duration = duration;
    readylist[job_cnt].timeleft = duration;
    readylist[job_cnt].priority = priority;
    readylist[job_cnt].status = READY;
    job_cnt++;
    return true;
}

/**
 * 计算调度信息。
 */
void summary() {
    int waittime = 0, runtime = 0;
    for (int i = 0; i < job_cnt; i++) {
        waittime += readylist[i].starttime - readylist[i].arrivetime;
        runtime += readylist[i].endtime - readylist[i].arrivetime;
    }
    printf("average waiting time: %.2lfs\n", (double) waittime / job_cnt);
    printf("average turnaround time: %.2lfs\n", (double) runtime / job_cnt);
}

/**
 * 先来先服务（FCFS）调度
 */
static int FCFS_sche() {
    int earliest = INT_MAX, next_job;
    for (int i = 0; i < job_cnt; i++) {
        if (readylist[i].status == READY) {
            if (readylist[i].arrivetime < earliest) {
                earliest = readylist[i].arrivetime;
                next_job = i;
            }
        }
    }
    if (earliest == INT_MAX) return -1;
    current_time = max(current_time, readylist[next_job].arrivetime);

    sche_duration = readylist[next_job].timeleft;
    readylist[next_job].starttime = current_time;
    readylist[next_job].endtime = current_time + readylist[next_job].duration;
    readylist[next_job].status = FINISH;
    readylist[next_job].timeleft = 0;
    return next_job;
}

/**
 * 短作业优先（SJF）调度
 */
static int SJF_sche() {
    int shortest = INT_MAX, next_job;
    for (int i = 0; i < job_cnt; i++) {
        if (readylist[i].status == READY && readylist[i].arrivetime <= current_time) {
            if (readylist[i].duration < shortest) {
                shortest = readylist[i].duration;
                next_job = i;
            }
        }
    }
    if (shortest == INT_MAX) return FCFS_sche();

    sche_duration = readylist[next_job].timeleft;
    readylist[next_job].starttime = current_time;
    readylist[next_job].endtime = current_time + readylist[next_job].duration;
    readylist[next_job].status = FINISH;
    readylist[next_job].timeleft = 0;
    return next_job;
}

/**
 * 优先级（PSA）调度
 */
static int PSA_sche() {
    int highest = INT_MAX, next_job;
    for (int i = 0; i < job_cnt; i++) {
        if (readylist[i].status == READY && readylist[i].arrivetime <= current_time) {
            if (readylist[i].priority < highest) {
                highest = readylist[i].priority;
                next_job = i;
            }
        }
    }
    if (highest == INT_MAX) return FCFS_sche();
    current_time = max(current_time, readylist[next_job].arrivetime);

    sche_duration = readylist[next_job].timeleft;
    readylist[next_job].starttime = current_time;
    readylist[next_job].endtime = current_time + readylist[next_job].duration;
    readylist[next_job].status = FINISH;
    readylist[next_job].timeleft = 0;
    return next_job;
}
