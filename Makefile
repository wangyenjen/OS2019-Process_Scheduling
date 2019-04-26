
# all: calc
all: process scheduler calc

process:
	gcc -O2 process.c common.c -o process

scheduler:
	gcc -O2 FIFO.c common.c merge_sort.c -o FIFO
	gcc -O2 RR.c common.c merge_sort.c -o RR
	gcc -O2 SJF.c common.c merge_sort.c heap.c -o SJF
	gcc -O2 PSJF.c common.c merge_sort.c heap.c -o PSJF
	gcc -O2 scheduler.c common.c -o scheduler

calc:
	gcc -O2 calc.c common.c merge_sort.c heap.c -o calc

clean:
	@rm process scheduler FIFO SJF RR PSJF calc
