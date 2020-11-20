#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MASTER 0      /* taskid of first task */
#define FROM_MASTER 1 /* setting a message type */
#define FROM_WORKER 2 /* setting a message type */

int main(int argc, char *argv[])
{
  int MATSIZE = atoi(argv[1]);
  int NRA = MATSIZE;
  int NCA = MATSIZE;
  int NCB = MATSIZE;
  int numtasks,              /* number of tasks in partition */
      taskid,                /* a task identifier */
      numworkers,            /* number of worker tasks */
      source,                /* task id of message source */
      dest,                  /* task id of message destination */
      mtype,                 /* message type */
      rows,                  /* rows of matrix A sent to each worker */
      averow, extra, offset, /* used to determine rows sent to each worker */
      i, j, k, rc;           /* misc */

  double *a = (double *)malloc(NRA * NCA * sizeof(double));
  double *b = (double *)malloc(NCA * NCB * sizeof(double));
  double *c = (double *)malloc(NRA * NCB * sizeof(double));

  /*
  double a[NRA][NCA],       
      b[NCA][NCB],           
      c[NRA][NCB]; */
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  //printf("Num task %i", numtasks);
  if (numtasks < 2)
  {
    printf("Need at least two MPI tasks. Quitting...\n");
    MPI_Abort(MPI_COMM_WORLD, rc);
    exit(1);
  }
  numworkers = numtasks - 1;

  /**************************** master task ************************************/
  if (taskid == MASTER)
  {
    //printf("mpi_mm has started with %d tasks.\n", numtasks);
    // printf("Initializing arrays...\n");
    srand(time(NULL));
    for (i = 0; i < MATSIZE; i++)
    {

      for (j = 0; j < MATSIZE; j++)
      {
        a[i * MATSIZE + j] = (double)(rand() % 100);
        b[i * MATSIZE + j] = (double)(rand() % 100);
        c[i * MATSIZE + j] = 0.0;
      }
    }

    /* Measure start time */
    double start = MPI_Wtime();

    /* Send matrix data to the worker tasks */
    averow = NRA / numworkers;
    extra = NRA % numworkers;
    offset = 0;
    mtype = FROM_MASTER;

    for (dest = 1; dest <= numworkers; dest++)
    {
      rows = (dest <= extra) ? averow + 1 : averow;
      //printf("Sending %d rows to task %d offset=%d\n", rows, dest, offset);
      MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
      MPI_Send(&rows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
      MPI_Send(a + (offset * MATSIZE + 0), rows * NCA, MPI_DOUBLE, dest, mtype,
               MPI_COMM_WORLD);
      MPI_Send(b, NCA * NCB, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);
      offset = offset + rows;
    }

    /* Receive results from worker tasks */
    mtype = FROM_WORKER;
    for (i = 1; i <= numworkers; i++)
    {
      source = i;
      MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
      MPI_Recv(&rows, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
      MPI_Recv(c + (offset * MATSIZE + 0), rows * NCB, MPI_DOUBLE, source, mtype,
               MPI_COMM_WORLD, &status);
        }
    /* Measure finish time */
    double finish = MPI_Wtime();
    printf("%f\t", finish - start);
  }

  /**************************** worker task ************************************/
  if (taskid > MASTER)
  {
    mtype = FROM_MASTER;
    MPI_Recv(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
    MPI_Recv(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
    MPI_Recv(a, rows * NCA, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD, &status);
    MPI_Recv(b, NCA * NCB, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD, &status);

    for (k = 0; k < NCB; k++)
    {
      for (i = 0; i < rows; i++)
      {
        for (j = 0; j < NCA; j++)
        {
          c[k * MATSIZE + i] += a[k * MATSIZE + j] * b[j * MATSIZE + i];
        }
      }
    }

    mtype = FROM_WORKER;
    MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
    MPI_Send(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
    MPI_Send(c, rows * NCB, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD);
  }
  MPI_Finalize();
}