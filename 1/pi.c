#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <pthread.h>

void check_argument(int argc){
    //check input argument
    if(argc < 2){
        printf("One argument excepted\n");
        exit(0);
    }
}

int get_processor(){
    int procs = get_nprocs();
    //printf("This system has %d processors configured and "
    //        "%d processors available.\n",
    //        get_nprocs_conf(), get_nprocs());
    
    return procs;
}

struct thread_data{
    int threadID;
    int tosses;
    int seed;
};

//in: tosses range, tid, seed
//out: the count of tosses incircle
void *toss_calculator(void *threadarg){
    struct thread_data *my_data;
    int  Incircle = 0;

    my_data = (struct thread_data*) threadarg;
    //get random float for tosses and check if they are in circle 
    for(int i=0; i<my_data->tosses ;i++){    
        float x = (float)rand_r(&my_data->seed)/(float)RAND_MAX;   
        float y = (float)rand_r(&my_data->seed)/(float)RAND_MAX;   
        //printf("thread id : %d   x : %f   y : %f\n",my_data->threadID,x,y );
        
        if(x*x + y*y <= 1){     //the area of square is 1 
            Incircle ++;
        } 
    }

    pthread_exit((void*)Incircle);
    return 0;
}

//This is the program to calculate pi by "Monte Catlo" method
int main(int argc, char *argv[]){
    check_argument(argc);
    
    int tosses = atoi(argv[1]); //get total tosses from user 
    int procs = get_processor();
    int Incircle[procs];
    double pi;
    clock_t start_time, end_time;
    float total_time;
    int tossesInGroup = tosses/procs;
    pthread_t threads[procs];   //try 2 threads
    struct thread_data thread_data_array[procs];
    void *status;
    int rc;
    int sum=0;
    
    srand(time(NULL));
    for(int i=0; i<procs ;i++){
        
        thread_data_array[i].threadID = i;
        thread_data_array[i].seed = rand();

        //also have to consider that number of process larger than tosses  
        if(i == procs-1){   //last thread should calculous the rest of tosses
            int lastTossesRange = tosses - tossesInGroup*(procs-1);
            thread_data_array[i].tosses = lastTossesRange;
            pthread_create(&threads[i], NULL, toss_calculator, (void*)&thread_data_array[i]);
        }
        else{
            thread_data_array[i].tosses = tossesInGroup;
            pthread_create(&threads[i], NULL, toss_calculator, (void*)&thread_data_array[i]); //identifier, thread attri, c routine, arg to routine
        }
    }    
    
    for(int i = 0; i<procs; i++){
        pthread_join(threads[i], (void**)&Incircle[i]);
        //printf("Incircle[%d] : %d\n", i, Incircle[i]);
        sum += Incircle[i];
    }
    //printf("sum : %d\n", sum);
    
    //formula : number in circle / total num of tosses = pi / 4 
    pi = ((double)sum/(double)tosses) * 4;
    printf("%.15f\n", pi);

    pthread_exit(NULL);
}
