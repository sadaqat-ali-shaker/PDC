#include <stdio.h>
#include <mpi.h>

#define ARRAY_SIZE 16

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int data[ARRAY_SIZE];
    double start, end;

    if (rank == 0) {
        printf("Process 0: Starting blocking communication\n");
        for (int i = 0; i < ARRAY_SIZE; i++)
            data[i] = i + 1;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0)
        start = MPI_Wtime();

    // Blocking communication
    if (rank == 0) {
        for (int i = 1; i < size; i++) {
            MPI_Send(data, ARRAY_SIZE, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Recv(data, ARRAY_SIZE, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else {
        MPI_Recv(data, ARRAY_SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 0; i < ARRAY_SIZE; i++)
            data[i] *= data[i];
        MPI_Send(data, ARRAY_SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        end = MPI_Wtime();
        printf("Process 0: Blocking communication time: %.6f seconds\n", end - start);
        printf("Process 0: Starting non-blocking communication\n");
        start = MPI_Wtime();
    }

    // Non-blocking communication
    MPI_Request req[2];

    if (rank == 0) {
        for (int i = 1; i < size; i++) {
            MPI_Isend(data, ARRAY_SIZE, MPI_INT, i, 0, MPI_COMM_WORLD, &req[0]);
            MPI_Irecv(data, ARRAY_SIZE, MPI_INT, i, 0, MPI_COMM_WORLD, &req[1]);
            MPI_Waitall(2, req, MPI_STATUSES_IGNORE);
        }
    } else {
        MPI_Irecv(data, ARRAY_SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, &req[0]);
        MPI_Wait(&req[0], MPI_STATUS_IGNORE);
        for (int i = 0; i < ARRAY_SIZE; i++)
            data[i] *= data[i];
        MPI_Isend(data, ARRAY_SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, &req[1]);
        MPI_Wait(&req[1], MPI_STATUS_IGNORE);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        end = MPI_Wtime();
        printf("Process 0: Non-blocking communication time: %.6f seconds\n", end - start);
    }

    MPI_Finalize();
    return 0;
}
