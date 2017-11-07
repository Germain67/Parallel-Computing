#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>

int* elems = NULL;

// Init array and fill elems with the values
int initArray()
{
  int arraySize, i;

  scanf("%d", &arraySize);

  printf("La taille du tableau est: %d\n", 2*arraySize);

  if(arraySize > 0)
  {
    elems = malloc(2*arraySize*sizeof(int));
    for(i=0; i< 2*arraySize; i++)
    {
      scanf("%d", &elems[i]);
    }
  }
  return 0;
}

int main (int argc, char const *argv[]){

  if (initArray() == 0){
    printf("Successfully initialized the array\n");
  }
  else{
    printf("Something bad happened =( \n");
  }

  free(elems);
  return 0;

}
