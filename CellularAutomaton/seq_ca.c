#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <string.h>
int main(int argc, char **argv)
{
  int i, j, m;
  int N = atoi(argv[1]);
  int iterations = atoi(argv[2]);
  int *A = (int *)malloc((N + 2) * sizeof(int));
  int *B = (int *)malloc((N + 2) * sizeof(int));

  srand(time(NULL));
  for (i = 1; i <= N; i++)
  {
    A[i] = rand() % 2;
  }
  /*
  for (i = 1; i <= N; i++)
  {
    printf("%i - ", A[i]);
  } */

  clock_t start, end;

  start = clock();

  for (int it = 0; it < iterations; it++)
  {
    m = 0;
    A[0] = A[N];
    A[N + 1] = A[1];
    for (i = 1; i <= N; i++)
    {
      if (A[i] == 1)
      {
        if (A[i + 1] == 1)
          B[i] = 1;
        else
        {
          B[i] = 0;
          m += 1;
        }
      }
      if (A[i] == 0)
      {
        if (A[i - 1] == 1)
        {
          B[i] = 1;
          m += 1;
        }
        else
          B[i] = 0;
      }
    }
    double avg = (double)m / N;
    //printf("avg: %f\n", avg);
    for (j = 0; j <= N + 1; j++)
    {
      A[j] = B[j];
    }
  }

  end = clock();
  /*
  for (i = 1; i <= N; i++)
  {
    printf("%i - ", B[i]);
  } */

  double t = ((double)(end - start)) / (CLOCKS_PER_SEC);
  printf("%f\t", t);
  return 0;
}