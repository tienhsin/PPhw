#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

int isprime(int n) {
    int i,squareroot;
    if (n>10) {
        squareroot = (int) sqrt(n);
        for (i=3; i<=squareroot; i=i+2)
            if ((n%i)==0)
                return 0;
        return 1;
    }
    else
        return 0;
}

int main(int argc, char *argv[])
{
    long long int pc;       /* prime counter */
    long long int foundone; /* most recent prime found */
    long long int n, limit;
    
    typedef struct{
        long long int proc_foundone;
        long long int proc_pc;
    }prime_ele;

    //variable for mpi 
    int my_rank; /* rank of process */
    int p;  /*number of processes*/
    int source; /*rank of sender*/
    int dest;   /*rank of recevier*/
    int tag=0; /*tag for message*/
    MPI_Status status;  /*return status for receive*/
    
    sscanf(argv[1],"%llu",&limit);    
    //printf("Starting. Numbers to be scanned= %lld\n",limit);

    /*MPI start*/
    MPI_Init( &argc, &argv);
    /*find out process rank*/
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    /* Find out number of processes */ 
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    
    /*creat type for struct prime_ele*/
    const int nitems=2;
    int blocklengths[2] = {1,1};
    MPI_Datatype types[2] = {MPI_LONG_LONG_INT, MPI_LONG_LONG_INT};
    MPI_Datatype mpi_prime_ele;
    MPI_Aint offsets[2];
    offsets[0] = offsetof(prime_ele, proc_foundone);
    offsets[1] = offsetof(prime_ele, proc_pc);
    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_prime_ele);
    MPI_Type_commit(&mpi_prime_ele);

    pc=4;     /* Assume (2,3,5,7) are counted here */
    if(my_rank != 0){
        prime_ele send;
        dest = 0;   //send to rank 0
        long long int p_end = (limit/p)*(my_rank+1), p_start = (limit/p)*my_rank+1; //setting limit range
        if(my_rank == (p-1)) //last proc
            p_end = limit;
        if(p_start%2 ==0)
            p_start++;
        //init 
        send.proc_pc = 0;
        send.proc_foundone = 0;
        //do something
        for (n=p_start; n<=p_end; n=n+2) {
            if (isprime(n)) {
                send.proc_pc++;
                send.proc_foundone = n;
            }           
        }
        //printf("p_start: %d, p_end: %d\n", p_start, p_end);
        //printf("Done. Largest prime in %d proc is %d\n Total primes %d\n", my_rank,send.proc_foundone,send.proc_pc);
        MPI_Send(&send, 1, mpi_prime_ele,dest,tag,MPI_COMM_WORLD); 
    
        }else{
           for(n=11; n<=(limit/p); n=n+2){
            if(isprime(n)){
                pc++;
                foundone = n;
            }
        }
        for(source=1; source<p; source++){
            prime_ele recv;
            MPI_Recv(&recv, 1, mpi_prime_ele, source, tag, MPI_COMM_WORLD, &status);
            pc += recv.proc_pc;
            if(recv.proc_foundone>foundone)
                foundone = recv.proc_foundone;
        }
        printf("Done. Largest prime is %lld Total primes %lld\n",foundone,pc);
    }
    MPI_Finalize();
    return 0;
} 
