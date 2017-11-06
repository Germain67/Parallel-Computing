#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>



int main (int argc, char const *argv[]){

  FILE *myFile;
  myFile = fopen("fichier_test", "r");
  if (myFile == NULL)
     exit(EXIT_FAILURE);

  size_t len = 0;
  int i;
  char* line = NULL;
  int lineSize = getline(&line, &len, myFile);
  int numberChar = 0;
  int previousBlank = 0;
  for(i=0;i<lineSize;i++)
  {
    if(line[i]==' ')
    {
      previousBlank=1;
    }
    else
    {
      if(previousBlank==1)
      {
        numberChar+=1;
        previousBlank=0;
      }
    }
  }
  printf("Nombre de space: %d\n", (numberChar));
  fclose(myFile);

  //-------------


  myFile = fopen("fichier_test", "r");
  if (myFile == NULL)
     exit(EXIT_FAILURE);

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
