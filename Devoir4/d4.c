#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

//Prototypes
void printMatrix(int* m, int n);
void generateGraph(int* matrix, int n);
char* load_program_source(const char *filename);
int showMatrix = 0;
int showTime = 0;
int verbose = 0;

//Main function
int main (int argc, char const *argv[]){
  struct timeval st, et;

  //Check command line arguments
  int argIndex;
  int n = 1000;
  for(argIndex = 1; argIndex < argc; argIndex++){
    if(strcmp(argv[argIndex], "-p") == 0){
      showMatrix = 1;
    }
    else if(strcmp(argv[argIndex], "-v") == 0){
      verbose = 1;
    }
    else if(strcmp(argv[argIndex], "-t") == 0){
      showTime = 1;
    }
    else if(strcmp(argv[argIndex], "-n") == 0){
      n = strtol(argv[argIndex+1], NULL, 10);
    }
  }

  char* programSource = load_program_source("kernel.cl");
  // This code executes on the OpenCL host
  //initialisation

  int* graph = malloc(n*n*sizeof(int));
  generateGraph(graph, n);

  if(showMatrix)
    printMatrix(graph, n);

  if(verbose)
    printf("Valeur de n : %d\n", n);

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
  if(verbose)
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

  if(verbose)
    printf("Number of devices = %d\n", (int)numDevices);

    // Allocate enough space for each device
  devices = (cl_device_id*)malloc(numDevices*sizeof(cl_device_id));

  // Fill in devices with clGetDeviceIDs()
  status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);

  for (int i=0; i<numDevices; i++){
      clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(Name), Name, NULL);
      if(verbose)
        printf("Name of device %d: %s\n", i, Name);
  }

  //-----------------------------------------------------
  // STEP 3: Create a context
  //-----------------------------------------------------
  if(verbose)
    printf("Création du contexte\n");
  fflush(stdout);
  cl_context context = NULL;
  // Create a context using clCreateContext() and
  // associate it with the devices
  context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);

  //-----------------------------------------------------
  // STEP 4: Create a command queue
  //-----------------------------------------------------
  if(verbose)
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
  if(verbose)
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
  if(verbose)
    printf("Ecriture dans les buffers\n");
  fflush(stdout);

  status = clEnqueueWriteBuffer(cmdQueue, bufferMatrix, CL_TRUE, 0, datasize, graph, 0, NULL, NULL);

  //-----------------------------------------------------
  // STEP 7: Create and compile the program
  //-----------------------------------------------------
  if(verbose)
    printf("CreateProgramWithSource\n");
  fflush(stdout);

  cl_program program = clCreateProgramWithSource(context,1, (const char**)&programSource, NULL, &status);

  //printf("%s\n",programSource);
  if(verbose)
    printf("Compilation\n");
  fflush(stdout);
  status = clBuildProgram(program, numDevices, devices, NULL, NULL, NULL);

  if (status) printf("ERREUR A LA COMPILATION: %d\n", status);

  if (status == CL_BUILD_PROGRAM_FAILURE) {
      // Determine the size of the log
      size_t log_size;
      clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

      // Allocate memory for the log
      char *log = (char *) malloc(log_size);

      // Get the log
      clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

      // Print the log
      printf("%s\n", log);
  }

  //-----------------------------------------------------
  // STEP 8: Create the kernel
  //-----------------------------------------------------

  cl_kernel kernel = NULL;
  if(verbose)
    printf("Création du kernel\n");
  fflush(stdout);
  kernel = clCreateKernel(program, "floidWarshall", &status);

  //-----------------------------------------------------
  // STEP 9: Set the kernel arguments
  //-----------------------------------------------------

  // Associate the input and output buffers with the
  // kernel
  // using clSetKernelArg()
  if(verbose)
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
  if(verbose)
    printf("Paramétrage de la structure des work-items\n");
  size_t MaxGroup;
  clGetDeviceInfo(devices[0],CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &MaxGroup, NULL);
  if(verbose)
    printf("CL_DEVICE_MAX_WORK_GROUP_SIZE = %d\n", (int) MaxGroup);

  size_t MaxItems[3];
  clGetDeviceInfo(devices[0],CL_DEVICE_MAX_WORK_ITEM_SIZES, 3*sizeof(size_t), MaxItems, NULL);
  if(verbose)
    printf("CL_DEVICE_MAX_WORK_ITEM_SIZES = (%d, %d, %d)\n", (int) MaxItems[0], (int)MaxItems[1], (int)MaxItems[2]);

  size_t globalWorkSize[2]={n, n};
  //size_t localWorkSize[3]={32, 32};
  // There are 'elements' work-items

  //-----------------------------------------------------
  // STEP 11: Enqueue the kernel for execution
  //-----------------------------------------------------

  // Execute the kernel by using
  // clEnqueueNDRangeKernel().
  // 'globalWorkSize' is the 1D dimension of the
  // work-items
  gettimeofday(&st,NULL);

  if(verbose)
    printf("Debut de l'appel\n");
  status = clEnqueueNDRangeKernel(cmdQueue, kernel, 2, NULL, globalWorkSize, NULL, 0, NULL, NULL);

  if(verbose)
    printf("Fin de l\'appel: status=%d\n", status);
  clFinish(cmdQueue);  // Pas nécessaire car la pile a été créée "In-order"

  /* status = clEnqueueNDRangeKernel( cmdQueue, kernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
  if(verbose)
    printf("Fin second appel: status=%d\n", status); */

  gettimeofday(&et,NULL);

  int elapsed = ((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec);
  if(showTime)
  {
    printf("Execution time : %d micro seconds; %f seconds\n", elapsed, ((float)elapsed/1000000.0));
  }
  //-----------------------------------------------------
  // STEP 12: Read the output buffer back to the host
  //-----------------------------------------------------

  //
  //Lecture de la matrice result
  //
  clEnqueueReadBuffer( cmdQueue, bufferMatrix, CL_TRUE, 0, datasize, graph, 0, NULL, NULL);

  //Step 12.5 : Print the result matrix
  if(showMatrix)
    printMatrix(graph, n);

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
  free(graph);
  //free(result);

  return 0;

}

//Generate the adjacence graph
void generateGraph(int* matrix, int n)
{
  int i, j;
  for(i=0; i < n; i++)
  {
    for(j=0; j < n; j++)
    {
      if(i == j){
        matrix[i*n + j] = 0;
      }
      else if((i + 1 == j) || (j == 0 && i == n-1)){
        matrix[i*n + j] = 1;
      }
      else{
        matrix[i*n + j] = n+1;
      }
    }
  }
}

//Print the matrix
void printMatrix(int* matrix, int n)
{
  int i, j;
  for(i=0; i<n; i++)
  {
  	for(j=0; j<n; j++)
  	{
  		printf("%d ", matrix[i*n + j]);
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
