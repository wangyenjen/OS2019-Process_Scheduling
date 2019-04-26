#define _GNU_SOURCE

#include <signal.h>
#include <sys/wait.h>
#include "include/common.h"
#include "include/merge_sort.h"
#include "include/priority.h"
#include "include/heap.h"

char N[MAX_PROCESS_NUM][MAX_PROCESS_NAME];
unsigned int R[MAX_PROCESS_NUM], T[MAX_PROCESS_NUM];

extern int nProcess;
extern int idxProcess[MAX_PROCESS_NUM];
extern pid_t pid[MAX_PROCESS_NUM];
extern int ptrReady;
extern int ptrRun;
extern int run;

struct Node *heap_root = NULL;

void sig_child(int signum)
{
    static int nFinish = 0;
    wait(NULL);
    ++nFinish;
    ++ptrRun;
    run = 0;
    if (nFinish == nProcess) exit(0);
}

void increase_next_process_priority() {
    int nextRun = heap_root->index;
    SET_PRIORITY(pid[nextRun], SCHED_FIFO, PRIORITY_INIT);
}

void decrease_next_process_priority() {
    if (!run || heap_root == NULL)
        return;
    int nextRun = heap_root->index;
    SET_PRIORITY(pid[nextRun], SCHED_FIFO, PRIORITY_LOW);
}

void run_next_process() {
    int nextRun = heap_root->index;
    heap_root = pop_heap(heap_root);
    SET_PRIORITY(pid[nextRun], SCHED_FIFO, PRIORITY_HIGH);	
    run = 1;						
}

void change_priority() {
    if (!run && heap_root != NULL)		
        run_next_process();
    if (run && heap_root != NULL)	
        increase_next_process_priority();
}

int main (void)
{    
    READ_INPUT();
    
    for (int i = 0; i < nProcess; ++i)
        idxProcess[i] = i;
    sort(nProcess, idxProcess, T);
    sort(nProcess, idxProcess, R);
    
    SIGACT_SIGCHLD();
    
    int nextReady = idxProcess[ptrReady];    

    for (int t = 0, i = nProcess; i > 0; ++t) {
        change_priority();
        while (ptrReady < nProcess && t == R[nextReady]) {
            decrease_next_process_priority();
            create_process(&pid[nextReady], N[nextReady], nextReady, T[nextReady]);
            heap_root = insert_heap(heap_root, T[nextReady], nextReady);
            nextReady = idxProcess[++ptrReady];
            change_priority();
        }
        unit_time();
    }
}
