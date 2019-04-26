#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "include/merge_sort.h"
#include "include/common.h"
#include "include/heap.h"

extern int nProcess;
extern int idxProcess[MAX_PROCESS_NUM];

char N[MAX_PROCESS_NUM][MAX_PROCESS_NAME];
unsigned int R[MAX_PROCESS_NUM], T[MAX_PROCESS_NUM];
int first_time = -1, last_time = 0;
unsigned int start_time[MAX_PROCESS_NUM], finish_time[MAX_PROCESS_NUM];

void calc_FIFO() {
    sort(nProcess, idxProcess, R);
    for (int i = 0; i < nProcess; ++i) {
        int nxt = idxProcess[i];
        start_time[nxt] = (last_time > R[nxt] ? last_time : R[nxt]);
        if (first_time == -1)
            first_time = start_time[nxt];
        last_time = finish_time[nxt] = start_time[nxt] + T[nxt];
        fprintf(stderr, "%s %u %u\n", N[nxt], start_time[nxt], finish_time[nxt]);
    }
}
void calc_SJF() {
    sort(nProcess, idxProcess, T);
    sort(nProcess, idxProcess, R);

    struct Node *heap_root = NULL;

    int *p = idxProcess;

    while (heap_root != NULL || p - idxProcess < nProcess) {
        if (heap_root == NULL && R[*p] > last_time)
            last_time = R[*p];
        while(p - idxProcess < nProcess && R[*p] <= last_time) {
            heap_root = insert_heap(heap_root, T[*p], *p);
            ++p;
        }
        int nxt = heap_root->index;
        heap_root = pop_heap(heap_root);
        start_time[nxt] = last_time;
        if (first_time == -1)
            first_time = start_time[nxt];
        last_time = finish_time[nxt] = last_time + T[nxt];
        fprintf(stderr, "%s %u %u\n", N[nxt], start_time[nxt], finish_time[nxt]);
    }
}

void calc_RR() {
    static const int MAX_QUEUE_SIZE = 1000000 + 7;
    static const int TIME_QUANTUM = 500;

    int que[MAX_QUEUE_SIZE], time_left[MAX_PROCESS_NUM];
    int ptrFront = 0, ptrBack = 0;

    memcpy(time_left, T, sizeof(int) * nProcess);
    memset(start_time, -1, sizeof(int) * nProcess);
    sort(nProcess, idxProcess, R);

    int nFinish = 0, *p = idxProcess, nxt = -1;
    while (nFinish < nProcess) {
        if (ptrBack == ptrFront && nxt == -1 && R[*p] > last_time)
            last_time = R[*p];

        while(p - idxProcess < nProcess && R[*p] <= last_time) {
            que[ptrBack++] = *p;
            ++p;
        }
        if (nxt != -1)
            que[ptrBack++] = nxt;

        nxt = que[ptrFront++];
        if (start_time[nxt] == -1) {
            start_time[nxt] = last_time;
            if (first_time == -1)
                first_time = start_time[nxt];
        }
        int exec_time =
            TIME_QUANTUM < time_left[nxt] ? TIME_QUANTUM : time_left[nxt];
        last_time += exec_time;
        time_left[nxt] -= exec_time;
        if (time_left[nxt] == 0) {
            finish_time[nxt] = last_time;
            ++nFinish;
            fprintf(stderr, "%s %u %u\n", N[nxt], start_time[nxt], finish_time[nxt]);
            nxt = -1;
        }
    }
}


void calc_PSJF() {
    sort(nProcess, idxProcess, T);
    sort(nProcess, idxProcess, R);

    struct Node *heap_root = NULL;

    int time_left[MAX_PROCESS_NUM];
    memcpy(time_left, T, sizeof(int) * nProcess);
    memset(start_time, -1, sizeof(int) * nProcess);

    int *p = idxProcess;
    while (heap_root != NULL || p - idxProcess < nProcess) {
        if (heap_root == NULL && R[*p] > last_time)
            last_time = R[*p];
        while (p - idxProcess < nProcess && R[*p] == last_time) {
            heap_root = insert_heap(heap_root, time_left[*p], *p);
            ++p;
        }
        int nxt = heap_root->index;
        heap_root = pop_heap(heap_root);
        if (start_time[nxt] == -1) {
            start_time[nxt] = last_time;
            if (first_time == -1)
                first_time = start_time[nxt];
        }
        if (p - idxProcess < nProcess && last_time + time_left[nxt] > R[*p]) {
            int exec_time = R[*p] - last_time;
            time_left[nxt] -= exec_time;
            last_time += exec_time;
            heap_root = insert_heap(heap_root, time_left[nxt], nxt);
        } else {
            last_time = finish_time[nxt] = last_time + time_left[nxt];
            time_left[nxt] = 0;
            fprintf(stderr, "%s %u %u\n", N[nxt], start_time[nxt], finish_time[nxt]);
        }
    }
}

int main(int argc, char **argv) {

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

    READ_INPUT();
    for (int i = 0; i < nProcess; ++i)
        idxProcess[i] = i;

    switch(scheduling_policy[0]) {                                               
        case 'F':
            calc_FIFO();
            break;                                                               
        case 'R':                                                                
            calc_RR();
            // fprintf(stderr, "-----------\n");
            // calc_easy_RR();
            break;                                                               
        case 'S':                                                                
            calc_SJF();
            break;                                                               
        case 'P':                                                                
            calc_PSJF();
            break;                                                               
    }                                        
    fprintf(stderr, "first time = %d\n", first_time);
    fprintf(stderr, "last time = %d\n", last_time);

    double exec_first_time = 1e100, exec_last_time = 0;
    double exec_start_time[MAX_PROCESS_NUM], exec_finish_time[MAX_PROCESS_NUM];
    char message[1000];
    FILE *pFile = fopen(argv[1], "r");
    while(fgets(message, 1000, pFile) != NULL) {
        // fprintf(stderr, "message = %s\n", message);
        if (strncmp(message, "[Project1]", 10) == 0) {
            strtok(message, " ");
            int idx = atoi(strtok(NULL, " "));
            // fprintf(stderr, "idx = %d\n", idx);
            exec_start_time[idx] = atof(strtok(NULL, " "));
            if (exec_start_time[idx] < exec_first_time)
                exec_first_time = exec_start_time[idx];
            exec_finish_time[idx] = atof(strtok(NULL, " "));
            exec_last_time = exec_finish_time[idx];
        }
    }
    fclose(pFile);

    fprintf(stderr, "exec first time = %.8f\n", exec_first_time);
    fprintf(stderr, "exec last time = %.8f\n", exec_last_time);

    double avg_unit_time
        = (exec_last_time - exec_first_time) / (last_time - first_time);
    fprintf(stderr, "avg unit time = %0.8f\n", avg_unit_time);
    double total_error = 0;
    for (int i = 0; i < nProcess; ++i) {
        double start_time_error = 
            (start_time[i] - first_time) * avg_unit_time - 
            (exec_start_time[i] - exec_first_time);
        double finish_time_error =
            (finish_time[i] - first_time) * avg_unit_time -
            (exec_finish_time[i] - exec_first_time);
        fprintf(stderr, "error of %s: start time: %1.8f, finish time: %1.8f\n",
                N[i], start_time_error, finish_time_error);
        total_error +=
            (start_time_error < 0 ? -start_time_error : start_time_error);
        total_error +=
            (finish_time_error < 0 ? -finish_time_error : finish_time_error);
    }
    fprintf(stderr, "total error = %1.8f\n", total_error);
    // for (int i = 0; i < nProcess; ++i) {
    //     int j = idxProcess[i];
    //     fprintf(stderr, "%s %u %u\n", N[j], start_time[j], finish_time[j]);
    // }
}

// int calc_easy_RR() {
//     static const int MAX_QUEUE_SIZE = 1000000 + 7;
//     static const int TIME_QUANTUM = 500;
// 
//     int que[MAX_QUEUE_SIZE], time_left[MAX_PROCESS_NUM];
//     int ptrFront = 0, ptrBack = 0;
// 
//     memcpy(time_left, T, sizeof(int) * nProcess);
//     memset(start_time, -1, sizeof(int) * nProcess);
// 
//     int last_time = 0, nFinish = 0, *p = idxProcess, nxt = -1;
//     while(nFinish < nProcess) {
// 
//         while(p - idxProcess < nProcess && R[*p] == last_time) {
//             que[ptrBack++] = *p;
//             ++p;
//         }
//         if (ptrFront < ptrBack) {
//             int nxt = que[ptrFront++];
//             if (start_time[nxt] == -1)
//                 start_time[nxt] = last_time;
//             int exec_time =
//                 TIME_QUANTUM < time_left[nxt] ? TIME_QUANTUM : time_left[nxt];
//             while(exec_time--) {
//                 --time_left[nxt];
//                 ++last_time;
//                 while(p - idxProcess < nProcess && R[*p] == last_time) {
//                     que[ptrBack++] = *p;
//                     ++p;
//                 }
//             }
//             while(p - idxProcess < nProcess && R[*p] == last_time) {
//                 que[ptrBack++] = *p;
//                 ++p;
//             }
//             if (time_left[nxt] == 0) {
//                 finish_time[nxt] = last_time;
//                 ++nFinish;
//                 fprintf(stderr, "%s %u %u\n", N[nxt], start_time[nxt], finish_time[nxt]);
//             } else {
//                 que[ptrBack++] = nxt;
//             }
//         } else
//             ++last_time;
//     }
// }
