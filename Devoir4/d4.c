#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <OpenCL/opencl.h>



// OpenCL kernel to perform an element-wise
// add of two arrays
const char* programSource =
"__kernel                                            \n"
"void vecadd(__global int *A,                        \n"
"            __global int *B,                        \n"
"            __global int *C)                        \n"
"{                                                   \n"
"                                                    \n"
"   // Get the work-item’s unique ID                 \n"
"   int idx = get_global_id(0);                      \n"
"                                                    \n"
"   // Add the corresponding locations of            \n"
"   // 'A' and 'B', and store the result in 'C'.     \n"
"   C[idx] = A[idx] + B[idx];                        \n"
"}                                                   \n"
;



//Prototypes
int** initMatrix(int n);
void freeMatrix(int** matrix, int n);
void printMatrix(int** m, int n);
void generateGraph(int** matrix, int n);

//Main function
int main (int argc, char const *argv[]){

  //initialisation
  int n = 4;
  int** graph = initMatrix(n);
  int** result = initMatrix(n);
  generateGraph(graph, n);
  printMatrix(graph, n);
  freeMatrix(graph, n);

  // Use this to check the output of each API call
  cl_int status;


  //-----------------------------------------------------
  // STEP 1: Discover and initialize the platforms
  //-----------------------------------------------------

  cl_uint numPlatforms = 0;
  cl_platform_id *platforms = NULL;

  // Use clGetPlatformIDs() to retrieve the number of
  // platforms
  status = clGetPlatformIDs(0, NULL, &numPlatforms);

  // Allocate enough space for each platform
  platforms = (cl_platform_id*)malloc(numPlatforms*sizeof(cl_platform_id));

  // Fill in platforms with clGetPlatformIDs()
  status = clGetPlatformIDs(numPlatforms, platforms,NULL);

  char Name[1000];
  clGetPlatformInfo(platforms[0], CL_PLATFORM_NAME, sizeof(Name), Name, NULL);
  printf("Name of platform : %s\n", Name);

  //-----------------------------------------------------
  // STEP 2: Discover and initialize the devices
  //-----------------------------------------------------

  cl_uint numDevices = 0;
  cl_device_id *devices = NULL;

  // Use clGetDeviceIDs() to retrieve the number of
  // devices present
  status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);

  printf("Number of devices = %d\n", (int)numDevices);

    // Allocate enough space for each device
  devices = (cl_device_id*)malloc(numDevices*sizeof(cl_device_id));

  // Fill in devices with clGetDeviceIDs()
  status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);

  for (int i=0; i<numDevices; i++){
      clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(Name), Name, NULL);
      printf("Name of device %d: %s\n", i, Name);
  }

  //-----------------------------------------------------
  // STEP 3: Create a context
  //-----------------------------------------------------

  cl_context context = NULL;

  // Create a context using clCreateContext() and
  // associate it with the devices
  context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);



  //-----------------------------------------------------
  // STEP 4: Create a command queue
  //-----------------------------------------------------

  cl_command_queue cmdQueue;

  // Create a command queue using clCreateCommandQueue(),
  // and associate it with the device you want to execute
  // on

  cmdQueue = clCreateCommandQueue(context, devices[0], 0, &status);



  return 0;

}

//Generate the adjacence graph
void generateGraph(int** matrix, int n)
{
  int i, j;
  for(i=0; i < n; i++)
  {
    for(j=0; j < n; j++)
    {
      if(i == j){
        matrix[i][j] = 0;
      }
      else if((i + 1 == j) || (j == 0 && i == n-1)){
        matrix[i][j] = 1;
      }
      else{
        matrix[i][j] = n+1;
      }
    }
  }
}

//Initialise the matrix in the memory
int** initMatrix(int n)
{
  int** res = malloc(n*sizeof(int*));
  int i, j;

  for(i=0; i < n; i++)
  {
    res[i]=malloc(n*sizeof(int));
  }
  return res;
}

//Free the matrix
void freeMatrix(int** matrix, int n)
{
  int i;
  for(i=0; i < n; i++)
  {
    free(matrix[i]);
  }
  free(matrix);
}

//Print the matrix
void printMatrix(int** matrix, int n)
{
  int i, j;
  for(i=0; i<n; i++)
  {
  	for(j=0; j<n; j++)
  	{
  		printf("%d ", matrix[i][j]);
  	}
  	printf("\n");
  }
  printf("\n");
}
