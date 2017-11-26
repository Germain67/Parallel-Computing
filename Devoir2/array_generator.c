#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

// [a, b[
int rand_a_b(int a, int b){
  return rand()%(b-a)+a;
}

void write_sorted_array(int size, FILE* fptr) {
  int num = 0;
  int i;
  for (i = 0; i < size; i++) {
    num = rand_a_b(num, num+10);
    fprintf(fptr, "%d\n", num);
  }
}

int main (int argc, char *argv[]) {
  int array_size;
  FILE *fptr;
  fptr = fopen("./array.txt", "w");

  if(fptr == NULL) {
    printf("Error!");
    exit(1);
  }

  printf("Array size ?\n");
  scanf("%d", &array_size);
  fprintf(fptr, "%d\n", array_size);

  write_sorted_array(array_size, fptr);
  write_sorted_array(array_size, fptr);

  fclose(fptr);
  printf("Done!\n");

  return 0;
}
