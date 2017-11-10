#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#include <time.h>

//Prototypes
int initArray();
void pMergeSeq(int* res);
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

    clock_t begin = clock();

    pMergeSeq(res);

    clock_t end = clock();


    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
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


//Main function to merge subarrays
void pMergeSeq(int* res)
{
  int i = 0;
  int j = 0;
  int k = 0;

  //Start the merge
  while (i < n && j < n)
  {

    if(elems[i] <= elems[j+n])
    {
      res[k] = elems[i];
      i++;
      k++;
    }
    else
    {
      res[k] = elems[j+n];
      j++;
      k++;
    }
  }

  //Finish the first array
  while (i < n)
  {
    res[k] = elems[i];
    i++;
    k++;
  }

  //Finish the second array
  while (j < n)
  {
    res[k] = elems[j];
    k++;
    j++;
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
