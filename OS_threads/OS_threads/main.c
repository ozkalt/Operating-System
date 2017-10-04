//
//  main.c
//  OS_threads
//
//  Created by Tuğba Özkal on 8.03.2017.
//  Copyright © 2017 Tuğba Özkal. All rights reserved.
//

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>

#define NUM_THREADS 4


void * busy_work(void * t){
    long tid;
    double result = 0.0;
    
    tid = (long) t;
    printf("Thread %ld starting ...\n", tid);
    
    for(int i = 0; i < 1000000; i++){
        result = result + sin(i) + tan(i);
    }
    printf("Thread %ld done. Result = %e\n", tid, result);
    pthread_exit((void *) t);
}

void * print_message_function(void * ptr){
    char * message;
    message = (char *) ptr;
    printf("\n %s \n", message);
    pthread_exit(NULL);
}

int main(int argc, const char * argv[]) {
    
    // program 2
    pthread_t thread[NUM_THREADS];
    pthread_attr_t attr;
    
    int rc;
    long t;
    void * status;
    
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for(t = 0; t < NUM_THREADS; t++){
        printf("Main: creating thread %ld\n", t);
        rc = pthread_create(&thread[t], &attr, busy_work, (void *) t);
        if(rc){
            printf("ERROR; return code from pthread create() is %d\n", rc);
            exit(-1);
        }
    }
    
    pthread_attr_destroy(&attr);
    for(t = 0; t < NUM_THREADS; t++){
        rc = pthread_join(thread[t], &status);
        if(rc){
            printf("ERROR; return code from pthread join() is %d\n", rc);
            exit(1);
        }
        
        printf("Main: completed join with thread %ld having a status of %ld \n", t, (long) status ) ;
    }
    
    printf("Main: program completed. Exiting.\n");
    pthread_exit(NULL);
    
    
    /*
     
    // program 1
     
    pthread_t thread1, thread2, thread3;
    char * message1 = "Hello ";
    char * message2 = "World ";
    char * message3 = "!...";
    
    if(pthread_create(&thread1, NULL, print_message_function, (void *) message1)){
        fprintf(stderr ,"pthread create failure\n");
        exit(1) ;
    }
    if(pthread_create(&thread2, NULL, print_message_function, (void *) message2)){
        fprintf(stderr ,"pthread create failure\n");
        exit(1) ;
    }
    if(pthread_create(&thread3, NULL, print_message_function, (void *) message3)){
        fprintf(stderr ,"pthread create failure\n");
        exit(1) ;
    }

    pthread_exit(NULL);
     */
}
