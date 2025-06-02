#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

#define SIZE 5

int main(int argc,char** argv){
    int rank,size;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    if(size<4){
        if(rank==0)
            printf("need atleast 4processes!\n");
        MPI_Finalize();
        return 0;
    }

    int array1[SIZE],array2[SIZE];

    if(rank==0){
        for(int i=0;i<SIZE;i++){
            array1[i]=i+1;
            array2[i]=(i+1)*10;
        }

        MPI_Send(array1,SIZE,MPI_INT,1,1000,MPI_COMM_WORLD);
        MPI_Send(array2,SIZE,MPI_INT,2,2000,MPI_COMM_WORLD);
        printf("process0 sent array1 to process1 and array2 to process2\n");
    }

    else if(rank==1){
        int recv_data[SIZE],result[SIZE];
        MPI_Status status;

        MPI_Recv(recv_data,SIZE,MPI_INT,0,1000,MPI_COMM_WORLD,&status);
        printf("process1 received data from process%dwith tag%d\n",status.MPI_SOURCE,status.MPI_TAG);

        for(int i=0;i<SIZE;i++)
            result[i]=recv_data[i]*recv_data[i];

        MPI_Send(result,SIZE,MPI_INT,3,3000,MPI_COMM_WORLD);
        printf("process1 sent squared data to process3\n");
    }

    else if(rank==2){
        int recv_data[SIZE],result[SIZE];
        MPI_Status status;

        MPI_Recv(recv_data,SIZE,MPI_INT,0,2000,MPI_COMM_WORLD,&status);
        printf("process2 received data from process%dwith tag%d\n",status.MPI_SOURCE,status.MPI_TAG);

        for(int i=0;i<SIZE;i++)
            result[i]=recv_data[i]*recv_data[i]*recv_data[i];

        MPI_Send(result,SIZE,MPI_INT,3,4000,MPI_COMM_WORLD);
        printf("process2 sent cubed data to process3\n");
    }

    else if(rank==3){
        int result1[SIZE],result2[SIZE];
        MPI_Status status;

        MPI_Recv(result1,SIZE,MPI_INT,MPI_ANY_SOURCE,3000,MPI_COMM_WORLD,&status);
        printf("process3 received squared data from process%dwith tag%d\n",status.MPI_SOURCE,status.MPI_TAG);

        MPI_Recv(result2,SIZE,MPI_INT,MPI_ANY_SOURCE,4000,MPI_COMM_WORLD,&status);
        printf("process3 received cubed data from process%dwith tag%d\n",status.MPI_SOURCE,status.MPI_TAG);

        printf("final result sat process3:\nsquares:");
        for(int i=0;i<SIZE;i++)
        printf("%d " ,result1[i]);
        printf("\ncubes:");
        for(int i=0;i<SIZE;i++)
        printf("%d ",result2[i]);
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}
