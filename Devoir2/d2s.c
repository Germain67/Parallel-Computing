#include <stdio.h>
#include <stdlib.h>
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
    struct timeval st, et;
		int* res = malloc(2*n*sizeof(int));
		gettimeofday(&st,NULL);
		pMergeSeq(res);
		gettimeofday(&et,NULL);
		int elapsed = ((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec);
		printf("Execution time : %d micro seconds\n", elapsed);
		printArray(res);
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

  printf("Array size: %d\n", 2*arraySize);

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
    res[k] = elems[j+n];
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
