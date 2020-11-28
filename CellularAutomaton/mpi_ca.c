#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char **argv)
{
  int N = atoi(argv[1]);
  int t = atoi(argv[2]);

  int numranks, rank, len;
  int tag = 0;

  char hostname[MPI_MAX_PROCESSOR_NAME];

  double startTime;
  double endTime;

  int *road = (int *)malloc((N + 2) * sizeof(int));

  srand(time(NULL));

  for (int i = 1; i <= n; i++)
  {
    road[i] = rand() % 2;
  }

  road[0] = road[n];
  road[N + 1] = road[1];

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Get_processor_name(hostname, &len);

  int *scRoad = (int *)malloc((N / numranks + 2) * sizeof(int));
  int *gtRoad = (int *)malloc((N / numranks + 2) * sizeof(int));
  int *road2 = (int *)malloc((N + 2) * sizeof(int));

  startTime = MPI_Wtime();

  MPI_Scatter(&road[rank * N / numranks], (N / numranks) + 2, MPI_INT, scRoad, (N / numranks) + 2, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);

  for (int r = 0; r < t; r++)
  {
    for (int i = 1; i < N / numranks + 1; i++)
    {
      if (scRoad[i] == 0)
      {
        if (scRoad[i - 1] == 1)
        {
          gtRoad[i] = 1;
        }
        else
        {
          gtRoad[i] = 0;
        }
      }
      else
      {
        if (scRoad[i + 1] == 0)
        {
          gtRoad[i] = 0;
        }
        else if (scRoad[i + 1] == 1)
        {
          gtRoad[i] = 1;
        }
      }
    }
    int prev = rank - 1;
    int next = rank + 1;
    if (prev < 0)
    {
      prev = numranks - 1;
    }
    if (next == numranks)
    {
      next = 0;
    }
    //printf("%d, %d, %d\n", rank, prev, next);
    MPI_Send(&gtRoad[1], 1, MPI_INT, prev, tag, MPI_COMM_WORLD);
    MPI_Send(&gtRoad[N / numranks], 1, MPI_INT, next, tag, MPI_COMM_WORLD);

    MPI_Recv(&gtRoad[0], 1, MPI_INT, prev, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&gtRoad[N / numranks + 1], 1, MPI_INT, next, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    MPI_Barrier(MPI_COMM_WORLD);

    tag = tag + 1;

    scRoad = gtRoad;
  }

  MPI_Gather(gtRoad, N / numranks + 1, MPI_INT, &road2[(N / numranks) * rank], N / numranks + 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);

  road2[N + 1] = road2[1];

  endTime = MPI_Wtime();

  if (rank == 0)
  {
    printf("%f", endTime - startTime);
  }

  MPI_Finalize();

  return 0;
}
