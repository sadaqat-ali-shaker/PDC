#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

int main(int argc,char** argv)
{

MPI_Init(&argc,&argv);
int rank,size,m=2;
MPI_Comm_rank(MPI_COMM_WORLD,&rank);
MPI_Comm_size(MPI_COMM_WORLD,&size);

if(size<4){
if(rank==0)

printf("minimum 4 processes required!\n");
MPI_Abort(MPI_COMM_WORLD,1);}
int next=(rank+1)%size;
int prev=(rank-1+size)%size;
int counter,cycle_count=0;
const int terminate=-1;

if(rank==0)
{
counter=0;
MPI_Send(&counter,1,MPI_INT,next,0,MPI_COMM_WORLD);
     while(cycle_count<m)
{
MPI_Recv(&counter,1,MPI_INT,prev,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
cycle_count++;

    if(cycle_count<m)
    {

counter++;
MPI_Send(&counter,1,MPI_INT,next,0,MPI_COMM_WORLD);}
}

int term_sig=terminate;
MPI_Send(&term_sig,1,MPI_INT,next,0,MPI_COMM_WORLD);
MPI_Recv(&term_sig,1,MPI_INT,prev,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
printf("process 0: terminated after %d cycles\n",m);}
   else{
           while(1){
         MPI_Recv(&counter,1,MPI_INT,prev,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
              if(counter==terminate){
         MPI_Send(&counter,1,MPI_INT,next,0,MPI_COMM_WORLD);
         printf("process %d: received termination\n",rank);
           break;}
        counter++;
                 MPI_Send(&counter,1,MPI_INT,next,0,MPI_COMM_WORLD);
                 printf("process %d: passed counter %d\n",rank,counter);
                }
                }
              MPI_Finalize();
             return 0;
            }
