__kernel void floidWarshall(__global int* matrix, int n )
{
  // Get the work-item’s unique ID
  int i, j, k;

  for(k= 0; k < n; k++)
  {
    for(j = get_global_id(0); j < n ; j++)
    {
      for(i = get_global_id(1); i < n ; i++)
      {
        //printf("i: %d j: %d k: %d n: %d", i, j, k, n);
        if(matrix[i*n+j] > (matrix[i*n+k] + matrix[k*n+j]))
        {
          matrix[i*n+j] = matrix[i*n+k] + matrix[k*n+j];
        }
      }
    }
  }


}
