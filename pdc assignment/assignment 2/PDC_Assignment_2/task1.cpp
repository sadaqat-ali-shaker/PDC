#include <mpi.h>
#include <stdio.h>

#define ARRAY_SIZE 16   // total elements in the array

int main(int argc,char** argv){
    int rank,size;
    int data[ARRAY_SIZE];

    MPI_Init(&argc,&argv);                 // initialize mpi
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);   // get current process rank
    MPI_Comm_size(MPI_COMM_WORLD,&size);   // get total number of processes

    if(size<2){
        if(rank==0)
            printf("you need at least 2 processes (1 master + 1 worker).\n");
        MPI_Finalize();
        return 0;
    }

    if(size>ARRAY_SIZE+1){
        if(rank==0)
            printf("too many processes. maximum allowed is 17 (1 master + 16 workers).\n");
        MPI_Finalize();
        return 0;
    }

    if(rank==0){
        // master process
        printf("master process......\n");
        printf("enter %d numbers:\n",ARRAY_SIZE);
        for(int i=0;i<ARRAY_SIZE;i++){
            scanf("%d",&data[i]);
        }

        // determine chunk sizes
        int base_chunk=ARRAY_SIZE/(size-1);  // basic elements per worker
        int extra=ARRAY_SIZE%(size-1);       // extra elements to distribute

        int offset=0;
        for(int i=1;i<size;i++){
            int count=base_chunk;
            if(i<=extra){
                count++;  // distribute the extra elements to first few workers
            }

            MPI_Send(&count,1,MPI_INT,i,0,MPI_COMM_WORLD);
            MPI_Send(&data[offset],count,MPI_INT,i,0,MPI_COMM_WORLD);

            offset+=count;
        }

        // receive squared results from workers
        offset=0;
        for(int i=1;i<size;i++){
            int count;
            MPI_Recv(&count,1,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            MPI_Recv(&data[offset],count,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            offset+=count;
        }

        // print the final array
        printf("final answere ouput:\n");
        for(int i=0;i<ARRAY_SIZE;i++){
            printf("%d ",data[i]);
        }
        printf("\n");

    }else{
        // worker process
        int count;
        MPI_Recv(&count,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

        int numbers[count];
        MPI_Recv(numbers,count,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

        for(int i=0;i<count;i++){
            numbers[i]=numbers[i]*numbers[i];
        }

        MPI_Send(&count,1,MPI_INT,0,0,MPI_COMM_WORLD);
        MPI_Send(numbers,count,MPI_INT,0,0,MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}