//
//  main.c
//  senkronizasyon
//
//  Created by Tuğba Özkal on 19.04.2017.
//  Copyright © 2017 Tuğba Özkal. All rights reserved.
//

#define _GNU_SOURCE

#include <stdio.h>
#include <pthread.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int s;

void sem_signal(int semid, int val){
    struct sembuf semafor;
    semafor.sem_num = 0;
    semafor.sem_op = val;
    semafor.sem_flg = 1;
    semop(semid, &semafor, 1);
}

void sem_waitt(int semid, int val){
    struct sembuf semafor;
    semafor.sem_num = 0;
    semafor.sem_op = (-1 * val);
    semafor.sem_flg = 1;
    semop (semid, &semafor, 1);
}


void createSemaphore (char* argv[]){
    int someKey = ftok(strcat(get_current_dir_name(), argv[0]) ,1);
    s = semget(someKey, 1, 0700|IPC_CREAT);
    semctl(s, 0, SETVAL, 1);
}

// int s; // currently not used
void* printThis(void* typ){
    int i, j;
    char* str = (char) typ == 'a' ? "abcdefghij" : "0123456789";
    
    // Create two types of content for print jobs
    for (i = 0 ; i < 100; i++){ // 100 separate print jobs
        //if(s > 0){
            //s--;
        sem_waitt(s, 1); // decrease
            for(j = 0; j < 10; j++){
                // of 10 pages each.
                    printf("%c", str[j]);
            // Each character represents a page
            }
            //s++;
        sem_signal(s, 1); // increase
        //}
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]){
    pthread_t a, b;
    setvbuf(stdout, (char*)NULL, _IONBF, 0);    // no−buffer printf
    
    //s = 1;
    
    createSemaphore(argv);
    
     
    // s=1; // currently not used
    
    //printf("I'm the NO−SYNC printer manager.\n");
    
    printf ("I'm the DUMMY−SYNC printer manager.\n");
    
    // Run two threads on printThis function with separate params
    pthread_create(&a, NULL, printThis, ( void *)'a');
    pthread_create(&b, NULL, printThis, (void *)'n');
    // Wait for the threads to finish
    
    pthread_join(a, NULL);
    pthread_join(b, NULL);
    
    semctl(s, 0, IPC_RMID, 0); // Delete s
    
    pthread_exit(NULL);
    return 0;
}
