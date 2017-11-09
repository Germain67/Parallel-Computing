#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#include <time.h>

//Prototypes
int initArray();
int max(int a, int b);
void invertVar(int* a, int* b);
int binarySearch(int x, int* T, int p, int r);
void pMerge(int* T, int p1, int r1, int p2, int r2, int* A, int p3);
void printArray(int* T);

//Global variables
int* elems = NULL;
int n = 0;


int main (int argc, char const *argv[]){

  if (initArray() == 0)
  {
    printf("Successfully initialized the array\n");
    //printArray(elems);

    int* res = malloc(2*n*sizeof(int));

    double start_time = omp_get_wtime();

    omp_set_dynamic(0);     // Explicitly disable dynamic teams
    omp_set_num_threads(24); // Use 4 threads for all consecutive parallel regions
    pMerge(elems, 0, n-1, n, 2*n-1, res, 0);

    double time_spent = omp_get_wtime() - start_time;

    printf("Execution time : %f seconds\n", time_spent);
    //printArray(res);

    free(res);
  }
  else
  {
    printf("Something bad happened =( \n");
  }

  free(elems);

  return 0;

}

// Init array and fill elems with the values
int initArray()
{
  int arraySize, i;

  scanf("%d", &arraySize);

  printf("La taille du tableau est: %d\n", 2*arraySize);

  if(arraySize > 0)
  {
    n = arraySize;
    elems = malloc(2*arraySize*sizeof(int));
    for(i=0; i< 2*arraySize; i++)
    {
      scanf("%d", &elems[i]);
    }
  }
  return 0;
}

//Invert variables values by using xor principle
void invertVar(int* a, int* b)
{
  if (a != b) {
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
  }
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
    invertVar(&p1, &p2);
    invertVar(&r1, &r2);
    invertVar(&n1, &n2);
  }
  if(n1 > 0)
  {
    int q1 = (p1+r1)/2;
    int q2 = binarySearch(T[q1], T, p2, r2);
    int q3 = p3 + (q1-p1) + (q2 - p2);
    A[q3] = T[q1];
    #pragma omp parallel sections
    {
      #pragma omp section
      pMerge(T, p1, q1-1, p2, q2-1, A, p3);
      #pragma omp section
      pMerge(T, q1+1, r1, q2, r2, A, q3+1);
    }
  }
}

//Print the array in stdout
void printArray(int* T)
{
  int i;
  for(i=0; i<2*n; i++)
  {
    printf("%d ", T[i]);
  }
  printf("\n");
}
