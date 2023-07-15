#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int myrank, size;
    MPI_Status status;
    double sTime, eTime;

    // This is for 2-d decomposition

    int process_rows = atoi(argv[2]);             //Number of processes in y axis
    int process_cols = atoi(argv[1]);             //Number of processes in x axis

    char *eptr;
    long long n = strtoll(argv[3], &eptr, 10);

    long long int rows = n / process_rows;        // Number of rows in a sub-domain
    long long int cols = n / process_cols;        // Number of cols in a sub-domain

    double matrix[rows][cols];                    // Each rank owns its subdomain
    double buffer[cols + 2];                      // for receiving 

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // initializing matrix
    srand((unsigned int)time(NULL) + myrank);
    for (long long int i = 0; i < rows; i++)
    {
        for (long long int j = 0; j < cols; j++)
        {
            int x = rand();
            int y = rand();
            if (y == 0)
                y = 1;
            matrix[i][j] = (double)x / (double)y;
        }
    }

    long long int count = ((((myrank / process_cols) + 1) * rows) + 1); //  total number of elements to be sent from each row to the row below it (here row refers to the processes which share boundaries left to right)
    count -= (myrank % process_cols) * cols;                            // decrease the number of elements sent by ranks present in that row to the left of that rank i.e., part of the subdomain which lies left to it.
    if (count > 0)
    {
        if (count >= (n / process_cols))
        {
            count = n / process_cols;                                   // can send only elements of its subdomain 
        }
    }
    else
        count = 0;

    if ((myrank / process_cols) == process_rows - 1)                    // count = 0 for last row processes
        count = 0;

    int recv_rank = myrank - (process_cols);                            // rank present just above it
    long long int recv_count = ((((recv_rank / process_cols) + 1) * (n / process_rows)) + 1); // Similar logic as count
    recv_count -= (recv_rank % process_cols) * (n / process_cols);
    if (recv_count > 0)
    {
        if (recv_count >= (n / process_cols))
        {
            recv_count = n / process_cols;
        }
    }
    else
        recv_count = 0;
    // recv_count = 0 for first row processes
    if (recv_rank < 0)
        recv_count = 0;

    sTime = MPI_Wtime();

    for (int i = 0; i < 20; i++)                                            // 20 iterations
    {
        if (count)
            MPI_Send(matrix[rows - 1], count, MPI_DOUBLE, myrank + process_cols, 1, MPI_COMM_WORLD);    

        if (recv_count)
            MPI_Recv(buffer, recv_count, MPI_DOUBLE, recv_rank, 1, MPI_COMM_WORLD, &status);

        // update matrix
        long long int comp = (myrank / process_cols) * rows - (myrank % process_cols) * cols;   //to compute the number of elements which need to be updated (lower triangular)

        // update in same subdomain using temporary matrix
        double temp[rows][cols];
        for (long long int i = 1; i < rows; i++)
        {
            long long int colsToUpdate = comp + i + 1;
            if (colsToUpdate >= cols)
                colsToUpdate = cols;
            for (long long int j = 0; j < colsToUpdate; j++)
            {
                temp[i][j] = matrix[i][j] - matrix[i - 1][j];
            }
        }
        // now copy the temp matrix into matrix.
        for (long long int i = 1; i < rows; i++)
        {
            long long int colsToUpdate = comp + i + 1;
            if (colsToUpdate >= cols)
                colsToUpdate = cols;
            for (long long int j = 0; j < colsToUpdate; j++)
            {
                matrix[i][j] = temp[i][j];
            }
        }

        // update using other subdomain
        for (long long int j = 0; j < recv_count; j++)
        {
            matrix[0][j] -= buffer[j];
            //  printf("[%d]--(%lld)%lf ",myrank,j,buffer[j]);
        }
    }

    eTime = MPI_Wtime();

    double mtime2d = eTime - sTime;
    double maxTime2d;
    MPI_Reduce(&mtime2d, &maxTime2d, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (!myrank)
        printf(" 2dtime for totalProcesses = %d for n= %lld = %lf\n", process_cols* process_rows, n, maxTime2d);

    // Now we will calculate for 1-d domain decomposition

    process_rows *= process_cols;                          // as it is row-wise decomposition
    process_cols = 1;

    rows = n / process_rows;
    cols = n / process_cols;

    double matrix1[rows][cols];
    double buffer1[cols+2];

    // initializing matrix1
    srand((unsigned int)time(NULL) + myrank);
    for (long long int i = 0; i < rows; i++)
    {
        for (long long int j = 0; j < cols; j++)
        {
            int x = rand();
            int y = rand();
             if(y==0)y=1;
            matrix1[i][j] = (double)x / (double)y;
        }
    }

    count = ((((myrank / process_cols) + 1) * (n / process_rows)) + 1);
    count -= (myrank % process_cols) * (n / process_cols);
    if (count > 0)
    {
        if (count >= (n / process_cols))
        {
            count = n / process_cols;
        }
    }
    else
        count = 0;

    if ((myrank / process_cols) == process_rows - 1)
        count = 0;                                          // count = 0 for last row processes

    recv_rank = myrank - (process_cols);
    recv_count = ((((recv_rank / process_cols) + 1) * (n / process_rows)) + 1);
    recv_count -= (recv_rank % process_cols) * (n / process_cols);
    if (recv_count > 0)
    {
        if (recv_count >= (n / process_cols))
        {
            recv_count = n / process_cols;
        }
    }
    else
        recv_count = 0;
    // recv_count = 0 for first row processes
    if (recv_rank < 0)
        recv_count = 0;


    sTime = MPI_Wtime();

    for (int i = 0; i < 20; i++)
    {
        if (count)
            MPI_Send(matrix1[rows - 1], count, MPI_DOUBLE, myrank + process_cols, 1, MPI_COMM_WORLD);

        if (recv_count)
            MPI_Recv(buffer1, recv_count, MPI_DOUBLE, recv_rank, 1, MPI_COMM_WORLD, &status);

        // update matrix1
        long long int comp = (myrank / process_cols) * rows - (myrank % process_cols) * cols;
        double temp[rows][cols];
        // update in same subdomain
        for (long long int i = 1; i < rows; i++)
        {
            long long int colsToUpdate = comp + i + 1;
            if (colsToUpdate >= cols)
                colsToUpdate = cols;
            for (long long int j = 0; j < colsToUpdate; j++)
            {
                temp[i][j] = matrix1[i][j] - matrix1[i - 1][j];
            }
        }
        for (long long int i = 1; i < rows; i++)
        {
            long long int colsToUpdate = comp + i + 1;
            if (colsToUpdate >= cols)
                colsToUpdate = cols;
            for (long long int j = 0; j < colsToUpdate; j++)
            {
                matrix1[i][j] = temp[i][j];
            }
        }
        // update using other subdomain
        for (long long int j = 0; j < recv_count; j++)
        {
            matrix1[0][j] -= buffer1[j];
            //  printf("[%d]--(%lld)%lf ",myrank,j,buffer1[j]);
        }
    }

    eTime = MPI_Wtime();

    double mtime1d = eTime - sTime;
    double maxTime1d;
    MPI_Reduce(&mtime1d, &maxTime1d, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (!myrank)
    printf(" 1dtime for totalProcess = %d for n= %lld = %lf\n", process_cols *process_rows, n, maxTime1d);

    MPI_Finalize();
    return 0;
}
