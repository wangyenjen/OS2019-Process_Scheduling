#define _GNU_SOURCE

#include <signal.h>
#include <sys/wait.h>
#include "include/common.h"
#include "include/merge_sort.h"
#include "include/priority.h"

#define MAX_QUEUE_SIZE (1000000 + 7)
#define TIME_QUANTUM 500

char N[MAX_PROCESS_NUM][MAX_PROCESS_NAME];
unsigned int R[MAX_PROCESS_NUM], T[MAX_PROCESS_NUM];

extern int nProcess;
extern int idxProcess[MAX_PROCESS_NUM];
extern pid_t pid[MAX_PROCESS_NUM];
extern int ptrReady;
extern int run;

int counting_time = 0, block_flag = 0;
int que[MAX_QUEUE_SIZE], ptrFront = 0, ptrBack = 0;

void push_queue(int x) {
    que[ptrBack++] = x;
}

int front_queue() {
    return que[ptrFront];
}

int size_queue() {
    return ptrBack - ptrFront;
}

int empty_queue() {
    return ptrFront < ptrBack ? 0 : 1;
}

void pop_queue() {
    if (ptrFront < ptrBack)
        ptrFront++;
}

void sig_child(int signum)
{
    static int nFinish = 0;
    wait(NULL);
    ++nFinish;
    pop_queue();
    run = 0;
    block_flag = 0;
    counting_time = 0;
    if (nFinish == nProcess) exit(0);
}

void increase_next_process_priority() {
    int nextRun = que[ptrFront + 1];
    SET_PRIORITY(pid[nextRun], SCHED_FIFO, PRIORITY_INIT);
}

void run_next_process() {
    counting_time = 0;
    int nextRun = que[ptrFront];
    if (T[nextRun] <= TIME_QUANTUM)
        block_flag = 1;
    SET_PRIORITY(pid[nextRun], SCHED_FIFO, PRIORITY_HIGH);	
    run = 1;						
}

void change_priority() {
    if (!run && size_queue() >= 1)		
        run_next_process();
    if (run && size_queue() >= 2)	
        increase_next_process_priority();
}

int main (void)
{    
    READ_INPUT();
    
    for (int i = 0; i < nProcess; ++i)
        idxProcess[i] = i;
    sort(nProcess, idxProcess, R);
    
    SIGACT_SIGCHLD();
    
    int nextReady = idxProcess[ptrReady];    

    for (int t = 0, i = nProcess; i > 0; ++t) {
        change_priority();
        while (ptrReady < nProcess && t == R[nextReady]) {
            create_process(&pid[nextReady], N[nextReady], nextReady, T[nextReady]);
            push_queue(nextReady);
            nextReady = idxProcess[++ptrReady];
            change_priority();
        }
        unit_time();
        counting_time++;
        if (!block_flag && run && counting_time == TIME_QUANTUM) { 
            int id = front_queue();
            pop_queue();
            T[id] -= TIME_QUANTUM;
            SET_PRIORITY(pid[id], SCHED_FIFO, PRIORITY_LOW);	
            push_queue(id);
            run = 0;
            change_priority();
        }
    }
}
