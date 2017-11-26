#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <string.h>
#include <mpi.h>


//Prototypes
int* initMatrice();
void pMergeSeq(int* res);
void printMatrix(int* T);

int n = -1;

MPI_Status status;

int main (int argc, char const *argv[]){

	// Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Print off a hello world message
    printf("-- Hello world from processor %s, rank %d"
           " out of %d processors\n",
           processor_name, world_rank, world_size);
		   
		   
	struct timeval start, stop;
	
	//Processor number indexes
	int source, dest;
	
	int rows, offset; 
	
	//Matrix declarations
	int* matrix1;
	int* matrix2;
	int* matrixResultat;
		   
	//Master   
	if(world_rank == 0)
	{
		printf("* Start matrix initialization\n");
		matrix1 = initMatrice();
		matrix2 = initMatrice();
		
		if (n != 0)
		{
			matrixResultat = (int* )malloc(n*n*sizeof(int));
			printf("\n");
			printf("* Successfully initialized the matrix\n");
			printf("* Matrix sizes: %d*%d\n", n,n);
			printf("\n");
			//Test if we have to print arrays
			if(argc > 1 && strcmp(argv[1], "-p") == 0)
			{
				printMatrix(matrix1);
				printMatrix(matrix2);
			}
			
			gettimeofday(&start, 0);
			//process
			
			int num_worker = world_size-1;
			rows = n/num_worker;
			offset = 0;
			
			//printf("Worker: %d / n: %d / rows: %d\n", num_worker, n, rows);

			for (dest=1; dest<=num_worker; dest++)
			{

				MPI_Send(&n, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);	//send array size
				MPI_Send(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD); //send offset
				MPI_Send(&rows, 1, MPI_INT, dest, 1, MPI_COMM_WORLD); //rsend rows variable
				MPI_Send(&matrix1[offset*n], rows*n, MPI_INT,dest,1, MPI_COMM_WORLD); //send the row
				MPI_Send(matrix2, n*n, MPI_INT, dest, 1, MPI_COMM_WORLD); //send matrix 2
				MPI_Send(matrixResultat, rows*n, MPI_INT, dest, 1, MPI_COMM_WORLD); //send the result matrix
				offset = offset + rows;
			}
			
			int i;
			
			 /* wait for results from all worker tasks */
			for (i=1; i<=num_worker; i++)
			{
			  source = i;

			  MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);

			  MPI_Recv(&rows, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);

			  MPI_Recv(&matrixResultat[offset*n], rows*n, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
			}
			
			printf("Result Matrix: \n");
			printMatrix(matrixResultat);
			
			gettimeofday(&stop, 0);
			
			free(matrix1);
			free(matrix2);
			free(matrixResultat);
    
		}
		else
		{
			printf("Something bad happened =( \n");
		}
	}
	
	//Worker 
	if(world_rank > 0)
	{
	
		//Tell that the source is the master
		source = 0;
		
		//Get n

		MPI_Recv(&n, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);

		MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
		
		MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);

		matrix1 = (int* )malloc(rows*n*sizeof(int));
		matrix2 = (int* )malloc(n*n*sizeof(int));
		matrixResultat = (int* )malloc(rows*n*sizeof(int));

		MPI_Recv(matrix1, rows*n, MPI_INT, source, 1, MPI_COMM_WORLD, &status);

		MPI_Recv(matrix2, n*n, MPI_INT, source, 1, MPI_COMM_WORLD, &status);

		MPI_Recv(matrixResultat, rows*n, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
		
		int i, j, k;

		/* Matrix multiplication */
		for (k=0; k<n; k++)
		{
			
			for (i=0; i<rows; i++) 
			{
				
				matrixResultat[i*n + k ] = 0;
				
				for (j=0; j<n; j++)
				{
					//printf("Value analysis: %d = %d + %d * %d\n", (matrixResultat[i*n + k ] + matrix1[i*n + j] * matrix2[j*n + k]), matrixResultat[i*n + k ], matrix1[i*n + j], matrix2[j*n + k]);
					matrixResultat[i*n + k] = matrixResultat[i*n + k] + matrix1[i*n + j] * matrix2[j*n + k];			
				}
			}
		}
		MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
		MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
		MPI_Send(matrixResultat, rows*n, MPI_INT, 0, 2, MPI_COMM_WORLD);
		
		 printf("-- Worker %s, rank %d, has finished :D\n",
           processor_name, world_rank, world_size);
	}

    // Finalize the MPI environment.
    MPI_Finalize();
 
  return 0;

}

// Init array and fill elems with the values
int* initMatrice()
{
	
  int matriceSize;
  int i;
  int j;
  int* matrix;
  
  if(n == -1)
  {
	 scanf("%d", &matriceSize);
  }


  if(matriceSize > 0)
  {
    n = matriceSize;
	
    matrix = (int* )malloc(n*n*sizeof(int));
	
    for(i=0; i < n; i++)
    {
		for(j=0; j < n; j++)
		{
			scanf("%d", &matrix[i*n+j]);
		}
    }
  }
  return matrix;
}


//Print the array in stdout
void printMatrix(int* m)
{
  int i, j;
  for(i=0; i<n; i++)
  {
	for(j=0; j<n; j++)
	{
		printf("%d ", m[i*n+j]);
	}
	printf("\n");
  }
  printf("\n");
}
