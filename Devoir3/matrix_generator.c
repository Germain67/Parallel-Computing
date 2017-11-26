#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>

int min;
int max;

// [a, b[
int rand_a_b(int a, int b){
  return rand()%(b-a)+a;
}

void write_matrix(int size, FILE* fptr) {
  int num;
  int i;
  //Fill 2 matrix of size 2^n * 2^n
  for (i = 0; i < size*size*2; i++) {
    fprintf(fptr, "%d ", rand_a_b(min, max));
  }
}

int main (int argc, char *argv[]) {
  struct stat st = {0};

  if (stat("./matrixs", &st) == -1) {
      mkdir("./matrixs", 0700);
  }

  char buffer[50];
  int array_size;

  printf("Enter n value for 2 matrixs of size (2^n)^2 : \n");
  scanf("%d", &array_size);
  sprintf (buffer, "./matrixs/matrix_2_pow_%d.txt", array_size);
  array_size = pow(2,array_size);
  FILE *fptr;
  fptr = fopen(buffer, "w");

  if(fptr == NULL) {
    printf("Error!");
    exit(1);
  }

  printf("Min bound ?\n");
  scanf("%d", &min);
  printf("Max bound ?\n");
  scanf("%d", &max);

  fprintf(fptr, "%d ", array_size);

  write_matrix(array_size, fptr);

  fclose(fptr);
  printf("Done!\n");

  return 0;
}
