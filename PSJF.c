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
extern int run;

int nFinish = 0;
int now_time = 0;
int counting_time = 0;
struct Node *heap_root = NULL;

#define BLOCK_SIGCHLD()				\
    sigset_t set, oset;				\
    do {					\
	sigfillset(&set);			\
	sigprocmask(SIG_BLOCK, &set, &oset);	\
    } while(0)

#define UNBLOCK_SIGCHLD() sigprocmask(SIG_SETMASK, &oset, NULL)

#define WAIT_CHILD() do {						\
	BLOCK_SIGCHLD();						\
	while (heap_root != NULL && waitpid(pid[heap_root->index], NULL, WNOHANG) != 0) { \
	    ++nFinish;							\
	    heap_root = pop_heap(heap_root);				\
	    run = 0;							\
	    if (nFinish == nProcess) exit(0);				\
	}								\
	UNBLOCK_SIGCHLD();						\
    } while(0)

void sig_child(int signum)
{
    wait(NULL);
    ++nFinish;
    heap_root = pop_heap(heap_root);
    run = 0;
    if (nFinish == nProcess) exit(0);
}

void increase_next_process_priority() {
    BLOCK_SIGCHLD();
    int i1 = heap_root->index;
    heap_root = pop_heap(heap_root);
    SET_PRIORITY(pid[i1], SCHED_FIFO, PRIORITY_HIGH);
    WAIT_CHILD();
    if (heap_root) {
        int i2 = heap_root->index;
        SET_PRIORITY(pid[i2], SCHED_FIFO, PRIORITY_INIT);
    }
    heap_root = insert_heap(heap_root, T[i1], i1);
    UNBLOCK_SIGCHLD();
}

void decrease_next_two_processes_priority() {
    BLOCK_SIGCHLD();
    if (!run || heap_root == NULL)
        return;
    int i1 = heap_root->index;
    heap_root = pop_heap(heap_root);
    T[i1] -= counting_time;
    WAIT_CHILD();
    if (heap_root) {
        int i2 = heap_root->index;
        SET_PRIORITY(pid[i2], SCHED_FIFO, PRIORITY_LOW);
    }
    SET_PRIORITY(pid[i1], SCHED_FIFO, PRIORITY_LOW);
    heap_root = insert_heap(heap_root, T[i1], i1);
    counting_time = 0;
    UNBLOCK_SIGCHLD();
}

void run_next_process() {
    BLOCK_SIGCHLD();
    int nextRun = heap_root->index;
    SET_PRIORITY(pid[nextRun], SCHED_FIFO, PRIORITY_HIGH);	
    run = 1;						
    counting_time = 0;
    UNBLOCK_SIGCHLD();
}

void change_priority() {
    WAIT_CHILD();
    BLOCK_SIGCHLD();
    if (!run && heap_root != NULL)		
        run_next_process();
    if (run && heap_root != NULL)	
        increase_next_process_priority();
    UNBLOCK_SIGCHLD();
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

    for (int t = 0; ; ++t) {
        change_priority();
        while (ptrReady < nProcess && t == R[nextReady]) {
            decrease_next_two_processes_priority();
            create_process(&pid[nextReady], N[nextReady], nextReady, T[nextReady]);
            heap_root = insert_heap(heap_root, T[nextReady], nextReady);
            nextReady = idxProcess[++ptrReady];
            change_priority();
        }
        unit_time();
        counting_time++;
        now_time++;
        WAIT_CHILD();
    }
}
