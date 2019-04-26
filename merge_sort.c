#include <stdio.h>

#define MAX_ARRAY_LENGTH (100000 + 7)

int tmp_array_for_sort[MAX_ARRAY_LENGTH];
void _sort(int lb, int rb, int idx[], unsigned int R[]) { // [lb, rb]
    if (lb >= rb)
	return;
	
    int mid = (lb + rb) >> 1;
	
    _sort(lb, mid, idx, R);
    _sort(mid + 1, rb, idx, R);

    int i1 = lb, i2 = mid + 1, p = lb;
    while (i1 <= mid || i2 <= rb) {
	if (i1 <= mid && i2 <= rb) {
	    if (R[idx[i1]] <= R[idx[i2]])
		tmp_array_for_sort[p++] = idx[i1++];
	    else
		tmp_array_for_sort[p++] = idx[i2++];
	}
	else if (i1 <= mid) 
	    tmp_array_for_sort[p++] = idx[i1++];
	else 
	    tmp_array_for_sort[p++] = idx[i2++];
    }

    int i;
    for (i = lb; i <= rb; i++)
	idx[i] = tmp_array_for_sort[i];
}

void sort(int n, int idx[], unsigned int R[]) {
    // idx: 0, 1, 2, ..., n;
    // sort idx according to R[idx];
    _sort(0, n - 1, idx, R);
}

int idx[MAX_ARRAY_LENGTH];
unsigned int R[MAX_ARRAY_LENGTH];

static int test() {
    int n = 5;
    for (int i = 0; i < n; i++) {
        idx[i] = i;
    }
    R[0] = 100, R[1] = 50, R[2] = 150, R[3] = 87, R[4] = 26;
    sort(n, idx, R);
    for (int i = 0; i < n; i++)
        printf("%d: %d\n", idx[i], R[idx[i]]);
    return 0;
}
