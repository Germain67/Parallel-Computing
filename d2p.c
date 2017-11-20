#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#include <time.h>

//Prototypes
int initArray(int** T);
int max(int a, int b);
void invertVar(int* a, int* b);
int binarySearch(int x, int* T, int p, int r);
void pMerge(int* T, int p1, int r1, int p2, int r2, int* A, int p3);
void printArray(int* T, int n);
void pMergeSeq(int* T, int p1, int r1, int p2, int r2, int* A, int p3);


int main (int argc, char const *argv[]){

  int* T;
  int n = initArray(&T);

  if (n > 0)
  {
    printf("Successfully initialized the array\n");

    int nbThreads[8] = { 1, 2, 4, 8, 12, 16, 24, 48 };
    int i;
    int j;


    for(i = 0; i < 8; i++){

      int* A = malloc(2*n*sizeof(int));
      omp_set_num_threads(nbThreads[i]); // Use nbThreads for all consecutive parallel regions
      double start_time = omp_get_wtime();
      //omp_set_dynamic(0);     // Explicitly disable dynamic teams
      #pragma omp parallel shared(T, A)
      {
        #pragma omp single nowait
          pMerge(T, 0, n-1, n, 2*n-1, A, 0);
      }

      double time_spent = omp_get_wtime() - start_time;
      printf("Execution time on %d threads : %f seconds\n", nbThreads[i], time_spent);
      printArray(A,n);
      free(A);

    }

    free(T);
  }
  else
  {
    printf("Something bad happened =( \n");
  }

  return 0;

}

// Init array and fill elems with the values
int initArray(int** T)
{
  int arraySize, i;

  scanf("%d", &arraySize);

  printf("La taille du tableau est: %d\n", 2*arraySize);

  if(arraySize > 0)
  {
    *T = malloc(2*arraySize*sizeof(int));
    for(i=0; i< 2*arraySize; i++)
    {
      scanf("%d", &(*T)[i]);
    }
  }
  return arraySize;
}



//Get the highest var between a and b
int max(int a, int b)
{
  return (a>b)?a:b;
}


//BinarySeach function used by pMerge
int binarySearch(int x, int* T, int p, int r)
{
  int low = p;
  int high = max(p, r+1);
  while(low < high)
  {
    int mid = (low + high)/2;
    if(x <= T[mid])
    {
      high = mid;
    }
    else
    {
      low = mid +1;
    }
  }
  return high;
}

//Main function to merge subarrays
void pMerge(int* T, int p1, int r1, int p2, int r2, int* A, int p3)
{
  int n1 = r1-p1+1;
  int n2 = r2-p2+1;
  if(n1 < n2)
  {
    int t;
    t = p1; p1 = p2; p2 = t;
    t = r1; r1 = r2; r2 = t;
    t = n1; n1 = n2; n2 = t;
    /* invertVar(&p1, &p2);
    invertVar(&r1, &r2);
    invertVar(&n1, &n2); */
  }
  if(n1 > 100)
  {
    int q1 = (p1+r1)/2;
    int q2 = binarySearch(T[q1], T, p2, r2);
    int q3 = p3 + (q1-p1) + (q2 - p2);
    A[q3] = T[q1];
    #pragma omp task
      pMerge(T, p1, q1-1, p2, q2-1, A, p3);
    #pragma omp task
      pMerge(T, q1+1, r1, q2, r2, A, q3+1);
  }
  else
  {
	  pMergeSeq(T, p1, r1, p2, r2, A, p3);
  }
}


void pMergeSeq(int* T, int p1, int r1, int p2, int r2, int* A, int p3)
{
  int i = p1;
  int j = p2;
  int k = p3;

  //Start the merge
  while (i <= r1 && j <= r2)
  {

    if(T[i] <= T[j])
    {
      A[k] = T[i];
      i++;
      k++;
    }
    else
    {
      A[k] = T[j];
      j++;
      k++;
    }
  }

  //Finish the first array
  while (i <= r1)
  {
    A[k] = T[i];
    i++;
    k++;
  }

  //Finish the second array
  while (j <= r2)
  {
    A[k] = T[j];
    k++;
    j++;
  }
}

//Print the array in stdout
void printArray(int* T, int n)
{
  int i;
  for(i=0; i<2*n; i++)
  {
    printf("%d ", T[i]);
  }
  printf("\n");
}
