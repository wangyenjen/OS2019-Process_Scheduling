#define _GNU_SOURCE

#include <signal.h>
#include <sys/wait.h>
#include "include/common.h"
#include "include/merge_sort.h"
#include "include/priority.h"

char N[MAX_PROCESS_NUM][MAX_PROCESS_NAME];
unsigned int R[MAX_PROCESS_NUM], T[MAX_PROCESS_NUM];

extern int nProcess;
extern int idxProcess[MAX_PROCESS_NUM];
extern pid_t pid[MAX_PROCESS_NUM];
extern int ptrReady;
extern int ptrRun;
extern int run;

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
    int nextRun = idxProcess[ptrRun + 1];
    SET_PRIORITY(pid[nextRun], SCHED_FIFO, PRIORITY_INIT);
}

void run_next_process() {
    int nextRun = idxProcess[ptrRun];			
    SET_PRIORITY(pid[nextRun], SCHED_FIFO, PRIORITY_HIGH);	
    run = 1;						
}

void change_priority() {
    if (!run && ptrRun < ptrReady)		
        run_next_process();
    if (run && ptrRun + 1 < ptrReady)	
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
            nextReady = idxProcess[++ptrReady];
            change_priority();
        }
        unit_time();
    }
}
