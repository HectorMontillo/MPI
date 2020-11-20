//#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <string.h>
int main(int argc, char **argv)
{
  int i, j, k;
  int N = atoi(argv[1]);
  int *A = (int *)malloc(N * N * sizeof(int));
  int *B = (int *)malloc(N * N * sizeof(int));
  int *C = (int *)malloc(N * N * sizeof(int));

  clock_t start, end;
  srand(time(NULL));
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++)
    {
      A[i * N + j] = rand() % 100;
      B[i * N + j] = rand() % 100;
      C[i * N + j] = 0;
    }

  start = clock();

  for (i = 0; i < N; ++i)
  {
    //printf("%i\n", omp_get_num_threads());
    for (j = 0; j < N; ++j)
    {
      for (k = 0; k < N; ++k)
      {
        C[i * N + j] += A[i * N + k] * B[k * N + j];
      }
    }
  }
  end = clock();

  /*
  for (i = 0; i < N; i++)
  {
    for (j = 0; j < N; j++)
    {
      printf("%i - ", C[i][j]);
    }
    printf("\n");
  }*/

  double t = ((double)(end - start)) / (CLOCKS_PER_SEC);
  printf("%f\t", t);
  return 0;
}