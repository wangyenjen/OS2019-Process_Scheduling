#include <assert.h>
#include <stdlib.h>
#define MAX_PROCESS_NUM 10000
#define MAX_NAME_LENGTH 16
#define MAX_SCHEDULE_NAME 10
#define MAX_PROCESS_NAME 40
#define NUM_SCHEDULING_TYPE 4
#define TIME_UNIT 1000000UL

#define ERR_EXIT(a) do {			\
	perror(a); exit(1);			\
    } while (0)

#define READ_INPUT() do {					\
	assert(scanf("%d", &nProcess) == 1);			\
	for (int i = 0; i < nProcess; ++i) {			\
	    assert(scanf("%s%u%u", N[i], &R[i], &T[i]) == 3);	\
	}							\
    } while(0)

#define SIGACT_SIGCHLD() do {			\
	struct sigaction act;			\
	act.sa_flags = 0;			\
	act.sa_handler = sig_child;		\
	sigfillset(&act.sa_mask);		\
	sigaction(SIGCHLD, &act, NULL);		\
    } while(0)

void unit_time();

void create_process (pid_t *pid, char *N, int id, int T);
