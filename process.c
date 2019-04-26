#define _GNU_SOURCE
// #define __USE_GNU

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "include/common.h"

static int string_to_int(char *s) { // base-10
    int res = 0;
    for (; *s; s++)
        res = res * 10 + (*s - '0');
    return res;
}

int main(int argc, char **argv) {
    
    if (argc != 4) {
        fprintf(stderr, "usage: ./this_program process_name process_idx exec_time\n");
        return 0;
    }
    
    pid_t pid = getpid();
    
    char *name = argv[1];
    int idx = string_to_int(argv[2]);
    int exec_time = string_to_int(argv[3]);

    printf("%s %d\n", name, pid);
    // fprintf(stderr, "[%d]%s %d\n", pid, name, exec_time);
    
    long start_time = syscall(352);
    for (int i = 0; i < exec_time; i++)
        unit_time();
    long end_time = syscall(352);

    syscall(351, pid, start_time, end_time, 11111);
    static const long BASE = 1000000000;
    fprintf(stderr, "[Project1] %d %ld.%09ld %ld.%09ld\n", idx, start_time / BASE, start_time % BASE, end_time / BASE, end_time % BASE);
    // printf("[Project1] %d %ld.%09ld %ld.%09ld\n", pid, start_time / BASE, start_time % BASE, end_time / BASE, end_time % BASE);

    // fprintf(stderr, "[%d]finish\n", pid);

    return 0;
}
