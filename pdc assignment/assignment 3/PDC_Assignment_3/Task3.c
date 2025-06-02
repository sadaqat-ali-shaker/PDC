// NOTE
// don't have 6 physical cores or processes use 6 logical cores/processes for solvin the problem or completing the task


#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define REQUIRED_PROCS 6 // 6 processs

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Verify process count
    if (size != REQUIRED_PROCS) {
        if (rank == 0) {
            printf("Error: Requires exactly %d processes\n", REQUIRED_PROCS);
        }
        MPI_Finalize();
        return 1;
    }

    // --- Original Task 3 Logic ---
    srand((unsigned int)(time(NULL) + rank * 100));  
    int my_number = rand() % 100 + 1;
    int all_numbers[REQUIRED_PROCS];
    
    // Timing variables
    double allgather_start, allgather_time;
    double reduce_start, reduce_time;

    // Allgather phase
    allgather_start = MPI_Wtime();
    MPI_Allgather(&my_number, 1, MPI_INT, 
                 all_numbers, 1, MPI_INT, MPI_COMM_WORLD);
    allgather_time = MPI_Wtime() - allgather_start;

    reduce_start = MPI_Wtime();
    int max_value;
    MPI_Reduce(&my_number, &max_value, 1, MPI_INT, 
              MPI_MAX, 0, MPI_COMM_WORLD);
    reduce_time = MPI_Wtime() - reduce_start;


    double sum;
    MPI_Allreduce(&my_number, &sum, 1, MPI_DOUBLE, 
                 MPI_SUM, MPI_COMM_WORLD);
    double avg = sum / (double)REQUIRED_PROCS; 

    // Synchronize all processes before printing
    MPI_Barrier(MPI_COMM_WORLD);

    // Output
    printf("Process %d numbers: ", rank);
    for(int i = 0; i < REQUIRED_PROCS; i++) {
        printf("%d ", all_numbers[i]);
    }
    printf("\n");

    if(rank == 0) {
        printf("Maximum: %d\n", max_value);
    }
    printf("Process %d Average: %.2f\n", rank, avg);

    // Timing report
    if(rank == 0) {
        printf("\nPerformance Metrics:\n");
        printf("Allgather: %.6fs (N=%d)\n", allgather_time, REQUIRED_PROCS);
        printf("Reduce:    %.6fs (N=1)\n", reduce_time);
    }

    MPI_Finalize();
    return 0;
}
