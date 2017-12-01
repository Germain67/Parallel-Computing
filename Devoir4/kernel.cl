__kernel void floidWarshall(__global int** matrix, int k, int n)
{
  // Get the work-item’s unique ID
  int i = get_global_id(0);
  int j = get_global_id(1);



  for(; k < n; k++)
  {
    for(; j < n ; j++)
    {
      for(; i < n ; i++)
      {
        if(matrix[i][j] > (matrix[i][k] + matrix[k][j]))
        {
          matrix[i][j] + matrix[i][k] + matrix[k][j];
        }
      }
    }
  }


}
