// Adaptation Note:

//Due to hardware limitations allowing only 2 parallel processes, this implementation:
//     • Uses 2 processes instead of 4
//     • Maintains the original 16-element array size
//     • Divides data into 8-element chunks per process (16 ÷ 2)
//   Reaming Flow of the program is same

#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    int rank, size;
    int data[16];        
    int local_data[8];   // Each process gets 8 integers (since we're using 2 processes)

    MPI_Init(&argc, &argv);               
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 

    if (size != 2) {
        if (rank == 0) {
            printf("This program needs exactly 2 processes.\n");
        }
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        // Initialize array
        for (int i = 0; i < 16; i++) {
            data[i] = i + 1;
        }
        printf("Process 0 initialized data:\n");
        for (int i = 0; i < 16; i++) {
            printf("%d ", data[i]);
        }
        printf("\n");
    }


    MPI_Scatter(data, 8, MPI_INT, local_data, 8, MPI_INT, 0, MPI_COMM_WORLD);

    // process revice data
    printf("Process %d received data: ", rank);
    for (int i = 0; i < 8; i++) {
        printf("%d ", local_data[i]);
    }
    printf("\n");

    // multiplies its chunk by 2
    for (int i = 0; i < 8; i++) {
        local_data[i] *= 2;
    }

    // Gather updated chunks back into data array in process 0
    MPI_Gather(local_data, 8, MPI_INT, data, 8, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Final array after gathering:\n");
        for (int i = 0; i < 16; i++) {
            printf("%d ", data[i]);
        }
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}
