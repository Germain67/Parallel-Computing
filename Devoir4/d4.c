#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

//Prototypes
int** initMatrix(int n);
void freeMatrix(int** matrix, int n);
void printMatrix(int** m, int n);
void generateGraph(int** matrix, int n);
char* load_program_source(const char *filename);

//Main function
int main (int argc, char const *argv[]){

  char* programSource = load_program_source("kernel.cl");
  // This code executes on the OpenCL host

  //initialisation
  int n = 4;
  int** graph = initMatrix(n);
  generateGraph(graph, n);
  int** result = initMatrix(n);
  generateGraph(result, n);

  printMatrix(graph, n);

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
  fflush(stdout);

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
  printf("Création du contexte\n");
  fflush(stdout);
  cl_context context = NULL;
  // Create a context using clCreateContext() and
  // associate it with the devices
  context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);

  //-----------------------------------------------------
  // STEP 4: Create a command queue
  //-----------------------------------------------------

  printf("Création de la file d'attente\n");
  fflush(stdout);
  cl_command_queue cmdQueue;

  // Create a command queue using clCreateCommandQueue(),
  // and associate it with the device you want to execute
  // on

  cmdQueue = clCreateCommandQueue(context, devices[0], 0, &status);

  //-----------------------------------------------------
  // STEP 5: Create device buffers
  //-----------------------------------------------------

  printf("Création des buffers\n");
  fflush(stdout);

  size_t datasize = n*n*sizeof(int);

  cl_mem bufferMatrix;  // Input array on the device

  //?!?cl_mem bufferResult;  // Output array on the device

  // Use clCreateBuffer() to create a buffer object (d_A)
  // that will contain the data from the host array A
  bufferMatrix = clCreateBuffer(context, CL_MEM_READ_WRITE, datasize, NULL, &status);

  // Use clCreateBuffer() to create a buffer object (d_B)
  // that will contain the data from the host array B
  //?!?bufferResult = clCreateBuffer(context, CL_MEM_READ_WRITE, datasize, NULL, &status);


  //-----------------------------------------------------
  // STEP 6: Write host data to device buffers
  //-----------------------------------------------------
  printf("Ecriture dans les buffers\n");
  fflush(stdout);

  status = clEnqueueWriteBuffer(cmdQueue, bufferMatrix, CL_TRUE, 0, datasize, graph, 0, NULL, NULL);

  //-----------------------------------------------------
  // STEP 7: Create and compile the program
  //-----------------------------------------------------

  printf("CreateProgramWithSource\n");
  fflush(stdout);

  cl_program program = clCreateProgramWithSource(context,1, (const char**)&programSource, NULL, &status);

  //printf("%s\n",programSource);

  printf("Compilation\n");
  fflush(stdout);
  status = clBuildProgram(program, numDevices, devices, NULL, NULL, NULL);

  if (status) printf("ERREUR A LA COMPILATION: %d\n", status);

  //-----------------------------------------------------
  // STEP 8: Create the kernel
  //-----------------------------------------------------

  cl_kernel kernel = NULL;

  printf("Création du kernel\n");
  fflush(stdout);
  kernel = clCreateKernel(program, "floidWarshall", &status);

  //-----------------------------------------------------
  // STEP 9: Set the kernel arguments
  //-----------------------------------------------------

  // Associate the input and output buffers with the
  // kernel
  // using clSetKernelArg()

  printf("Passage des paramètres\n");
  fflush(stdout);

  status  = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &bufferMatrix);
  status  = clSetKernelArg(kernel, 1, sizeof(cl_int), (void*) &n);
  //status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*) &bufferResult);


  //-----------------------------------------------------
  // STEP 10: Configure the work-item structure
  //-----------------------------------------------------

  // Define an index space (global work size) of work items for execution.
  // A workgroup size (local work size) is not required, but can be used.

  size_t MaxGroup;
  clGetDeviceInfo(devices[1],CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &MaxGroup, NULL);
  printf("CL_DEVICE_MAX_WORK_GROUP_SIZE = %d\n", (int) MaxGroup);


  size_t MaxItems[2];
  clGetDeviceInfo(devices[1],CL_DEVICE_MAX_WORK_ITEM_SIZES, 2*sizeof(size_t), MaxItems, NULL);
  printf("CL_DEVICE_MAX_WORK_ITEM_SIZES = (%d, %d)\n", (int) MaxItems[0], (int)MaxItems[1]);



  size_t globalWorkSize[2]={n, n};
  size_t localWorkSize[3]={20,20};
  // There are 'elements' work-items

  //-----------------------------------------------------
  // STEP 11: Enqueue the kernel for execution
  //-----------------------------------------------------

  // Execute the kernel by using
  // clEnqueueNDRangeKernel().
  // 'globalWorkSize' is the 1D dimension of the
  // work-items

  printf("Debut des appels\n");
  status = clEnqueueNDRangeKernel(cmdQueue, kernel, 2, NULL, globalWorkSize, NULL, 0, NULL, NULL);

  printf("Fin premier appel: status=%d\n", status);
  clFinish(cmdQueue);  // Pas nécessaire car la pile a été créée "In-order"

  status = clEnqueueNDRangeKernel( cmdQueue, kernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
  printf("Fin second appel: status=%d\n", status);

  //-----------------------------------------------------
  // STEP 12: Read the output buffer back to the host
  //-----------------------------------------------------

  //
  //Lecture de la matrice result
  //
  clEnqueueReadBuffer( cmdQueue, bufferMatrix, CL_TRUE, 0, datasize, result, 0, NULL, NULL);

  //-----------------------------------------------------
  // STEP 13: Release OpenCL resources
  //-----------------------------------------------------

  // Free OpenCL resources
  clReleaseKernel(kernel);
  clReleaseProgram(program);
  clReleaseCommandQueue(cmdQueue);
  clReleaseMemObject(bufferMatrix);
  //?!? clReleaseMemObject(bufferResult);
  clReleaseContext(context);
  free(platforms);
  free(devices);
  freeMatrix(graph, n);
  freeMatrix(result, n);

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

// Lecture du fichier source
char* load_program_source(const char *filename) {
    FILE *fp;
    char *source;
    int sz=0;
    struct stat status;

    fp = fopen(filename, "r");
    if (fp == 0){
        printf("Echec\n");
        return 0;
    }

    if (stat(filename, &status) == 0)
        sz = (int) status.st_size;

    source = (char *) malloc(sz + 1);
    fread(source, sz, 1, fp);
    source[sz] = '\0';
    return source;
}
