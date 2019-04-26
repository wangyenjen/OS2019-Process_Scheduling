#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sched.h>
#include "include/common.h"
#include "include/priority.h"

int main() {
    
    char scheduling_policy[MAX_SCHEDULE_NAME];
    assert(scanf("%s", scheduling_policy) == 1);

    const char scheduling_types[NUM_SCHEDULING_TYPE][MAX_SCHEDULE_NAME]
	= { "FIFO", "RR", "SJF", "PSJF" };
    
    for (int i = 0; i <= NUM_SCHEDULING_TYPE; ++i) {
	if (i == NUM_SCHEDULING_TYPE) {
	    fprintf(stderr, "no such scheduling type\n");
	    return 1;
	}
	if (strcmp(scheduling_policy, scheduling_types[i]) == 0) break;
    }

    USE_CPU(getpid(), 0);
    SET_PRIORITY(getpid(), SCHED_FIFO, PRIORITY_INIT);

    fflush(stdin);
    switch(scheduling_policy[0]) {
    case 'F':
	execl("./FIFO", "./FIFO", (char *)0); 
	break;
    case 'R': 
	execl("./RR", "./RR", (char *)0); 
	break;
    case 'S': 
	execl("./SJF", "./SJF", (char *)0); 
	break;
    case 'P': 
	execl("./PSJF", "./PSJF", (char *)0); 
	break;
    }
    
    return 0;
}
