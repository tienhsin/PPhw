#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define PI 3.1415926535

int main(int argc, char **argv) 
{
    long long i, num_intervals;
    double rect_width, area, sum, x_middle; 

     //variable for mpi 
    int my_rank; /* rank of process */
    int p;  /*number of processes*/
    int source; /*rank of sender*/
    int dest;   /*rank of recevier*/
    int tag=0; /*tag for message*/
    char message[100];   /*storage for message*/
    MPI_Status status;  /*return status for receive*/
    
    sscanf(argv[1],"%llu",&num_intervals);
    rect_width = PI / num_intervals;
        
    /*MPI start*/
    MPI_Init( &argc, &argv);
    /*find out process rank*/
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    /* Find out number of processes */ 
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    
    sum = 0;
    if(my_rank !=0){
        long long int start=(num_intervals/p)*my_rank+1, end=(num_intervals/p)*(my_rank+1);
        if(my_rank == (p-1))
            end = num_intervals;
        double send = 0;
        dest = 0;
        for(i=start; i<=end;i++){
            x_middle = (i-0.5) * rect_width;
            area = sin(x_middle) * rect_width;
            send += area;
        }
        MPI_Send(&send, 1, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD); 
    }else{
        for(i = 1; i <= num_intervals/p; i++) {
            /* find the middle of the interval on the X-axis. */ 
            x_middle = (i - 0.5) * rect_width;
            area = sin(x_middle) * rect_width; 
            sum = sum + area;
        }
        for(source=1;source<p;source++){
            double recv;
            MPI_Recv(&recv, 1, MPI_DOUBLE, source, tag, MPI_COMM_WORLD, &status);
            sum += recv;
        }
        printf("The total area is: %f\n", (float)sum);
    }
    MPI_Finalize();
    return 0;
}   
