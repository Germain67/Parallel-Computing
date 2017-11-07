#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>

int* array1 = NULL;
int* array2 = NULL;

int initArrays()
{
  char fileBuffer[1024];

  if (fgets(fileBuffer, sizeof(fileBuffer), stdin) == NULL)
      return -1; // Input error / EOF
  printf("File content : %s\n", fileBuffer); // ** will help checking for the presence of white spaces.

  //read file into array
  int numberArray[numberChar];


  for (i = 0; i < numberChar; i++)
  {
      fscanf(myFile, "%d", &numberArray[i]);
  }

  for (i = 0; i < numberChar; i++)
  {
      printf("Number is: %d\n\n", numberArray[i]);
  }

  fclose(myFile);
  return 0;
}

int main (int argc, char const *argv[]){
  if (initArrays() == 0){
    printf("Successfully initialized the arrays\n");
  }
  else{
    printf("Something bad happened =( \n");
  }
  return 0;
}
