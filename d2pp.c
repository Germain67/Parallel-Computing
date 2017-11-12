#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <omp.h>

#ifndef max
	#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

int binary_search(int x, int* T, int p, int r) {
	int low = p;
	int high = max(p, r+1);
	int mid;

	while(low < high) {
		mid = (low+high)/2;
		if(x <= T[mid]) {
			high = mid;
		} else {
			low = mid+1;
		}
	}
	return high;
}

void p_merge(int* T, int p1, int r1, int p2, int r2, int* A, int p3) {

		int n1 = r1 - p1 + 1;
		int n2 = r2 - p2 + 1;

		int temp;

		if(n1 < n2) { //ensure that n1 >= n2
			temp=p1; p1=p2; p2=temp; // exchange p1 with p2
			temp=r1; r1=r2; r2=temp; // exchange r1 with r2
			temp=n1; n1=n2; n2=temp; // exchange n1 with n2
		}
		if(n1 != 0) {// both empty ?
			int q1 = (p1+r1)/2;

			int q2 = binary_search(T[q1], T, p2, r2);
			int q3 = p3+(q1-p1)+(q2-p2);
			A[q3] = T[q1];

			#pragma omp task
				p_merge(T, p1, q1-1, p2, q2-1, A, p3);

			#pragma omp task
				p_merge(T, q1+1, r1, q2, r2, A, q3+1);
		}
}


int main(int argc, char* argv[]) {
 	omp_set_num_threads(atoi(argv[1]));

	// Initialisation
	int n;
	scanf("%d", &n);

	int* A = malloc(2*n*sizeof(int));
	int* T = malloc(2*n*sizeof(int));
	int i;
	for(i=0; i < 2*n; i++) {
		scanf("%d", &T[i]);
	}
	printf("Start merging\n");
	// Merge
	double start_time = omp_get_wtime();
	#pragma omp parallel shared(T, A)
	{
		#pragma omp single
		p_merge(T, 0, n-1, n, 2*n-1, A, 0);
	}

	double time = omp_get_wtime() - start_time;
	printf("(Parallel n=%d, nbThreads:%d) Time: \t %f \n", n, time, atoi(argv[1]));

	free(A);
	free(T);

	return 0;
}
