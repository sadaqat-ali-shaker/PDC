#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ARRAY_SIZE 16

void print_array(int* arr, int size) {
    for(int i=0;i<size;i++) 
    printf("%d ", arr[i]);
    printf("\n");
}

int main(int argc,char** argv) {
    int rank,size;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    if(size>16 && rank==0) {
        printf("maximum 16 processes allowed!\n");
        MPI_Abort(MPI_COMM_WORLD,1);
    }

    int chunk_size=ARRAY_SIZE/size+(rank<ARRAY_SIZE%size);
    int chunk=(int)malloc(chunk_size*sizeof(int));
    MPI_Request *requests=NULL;

    if(rank==0) {
        int data[ARRAY_SIZE],result[ARRAY_SIZE];
        for(int i=0;i<ARRAY_SIZE;i++) data[i]=i+1;

        printf("\n16 integers\n");
        print_array(data,ARRAY_SIZE);

        // master distributes data
        requests=(MPI_Request*)malloc((size-1)*sizeof(MPI_Request));
        int sent=chunk_size;
        
        printf("\nmaster distributing data \n");
        for(int i=1;i<size;i++) {
            int worker_size=ARRAY_SIZE/size+(i<ARRAY_SIZE%size);
            MPI_Isend(&data[sent],worker_size,MPI_INT,i,0,MPI_COMM_WORLD,&requests[i-1]);
            printf("[master] sent %d elements to process%d: ",worker_size,i);
            print_array(&data[sent],worker_size);
            sent+=worker_size;
        }

        // master processes its own chunk
        for(int i=0;i<chunk_size;i++) result[i]=data[i]*data[i];
        MPI_Waitall(size-1,requests,MPI_STATUSES_IGNORE);
        free(requests);
        printf("\n[master] all data distribution complete!\n");

        // master collects results
        requests=(MPI_Request*)malloc((size-1)*sizeof(MPI_Request));
        int received=chunk_size;
        
        printf("\nmaster collecting results\n");
        for(int i=1;i<size;i++) {
            int worker_size=ARRAY_SIZE/size+(i<ARRAY_SIZE%size);
            MPI_Irecv(&result[received],worker_size,MPI_INT,i,0,MPI_COMM_WORLD,&requests[i-1]);
            received+=worker_size;
        }
        MPI_Waitall(size-1,requests,MPI_STATUSES_IGNORE);
        free(requests);
        printf("\n[master] all results received!\n");

        printf("\nfinal result\n");
        print_array(result,ARRAY_SIZE);
    }
    else {
        MPI_Request req[2];
        
        // worker receives data
        MPI_Irecv(chunk,chunk_size,MPI_INT,0,0,MPI_COMM_WORLD,&req[0]);
        MPI_Wait(&req[0],MPI_STATUS_IGNORE);
        printf("[process %d] received %d elements: ",rank,chunk_size);
        print_array(chunk,chunk_size);

        // worker processes data
        for(int i=0;i<chunk_size;i++) chunk[i]*=chunk[i];
        printf("[process %d] computed squares: ",rank);
        print_array(chunk,chunk_size);

        // worker sends back results
        MPI_Isend(chunk,chunk_size,MPI_INT,0,0,MPI_COMM_WORLD,&req[1]);
        MPI_Wait(&req[1],MPI_STATUS_IGNORE);
        printf("[process %d] sent back %d elements\n",rank,chunk_size);
    }

    free(chunk);
    MPI_Finalize();
    return 0;
}
