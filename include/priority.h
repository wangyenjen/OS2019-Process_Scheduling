#define _GNU_SOURCE

#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <stdlib.h>

// priority value: [1, 99]
#define PRIORITY_INIT (int)50
#define PRIORITY_LOW (int)10
#define PRIORITY_HIGH (int)80

#define PRINT_ERROR(name) do {						\
        fprintf(stderr, "[Error] %s: %d - ", __PRETTY_FUNCTION__, __LINE__); \
        perror(name);							\
        exit(EXIT_FAILURE);						\
    } while (0)

#define USE_CPU(pid, x) do {						\
	cpu_set_t mask;							\
	CPU_ZERO(&mask);						\
	CPU_SET((x), &mask);						\
	if (sched_setaffinity((pid), sizeof(mask), &mask) != 0) {	\
	    PRINT_ERROR("sched_setaffinity");				\
	}								\
    } while (0)

#define SET_PRIORITY(pid, policy, priority) do {			\
	/* policy: SCHED_FIFO, SCHED_RR, SCHED_BATCH */			\
	/* real time scheduler policy (require root permission): SCHED_FIFO, SCHED_RR */ \
	/* non-real time scheduler policy: SCHED_BATCH */		\
	struct sched_param param;					\
	param.sched_priority = priority;				\
	if (sched_setscheduler(pid, policy, &param) != 0) {		\
	    PRINT_ERROR("sched_setscheduler");				\
	}								\
    } while (0)

/*
  static int test() {
  USE_CPU(getpid(), 1);
  SET_PRIORITY(getpid(), SCHED_FIFO, PRIORITY_INIT);
  while (1) {
  volatile unsigned long i; 
  for(i = 0; i < 1000000UL; i++);
  }
  return 0;
  }
*/
