__kernel void floidWarshall(__global int* matrix, int n, int k)
{
  // Get the work-item’s unique ID
  int i = get_global_id(1);
  int j = get_global_id(0);
  if(matrix[i*n+j] > (matrix[i*n+k] + matrix[k*n+j]))
  {
    matrix[i*n+j] = matrix[i*n+k] + matrix[k*n+j];
  }
}
