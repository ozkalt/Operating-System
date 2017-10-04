//
//  main.c
//  semafor
//
//  Created by Tuğba Özkal on 18.04.2017.
//  Copyright © 2017 Tuğba Özkal. All rights reserved.
//


/* to be able to involve an integer variable in strcat */
#define _GNU_SOURCE

/* for shared memory and semaphores*/
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>

/* for handling signals */
#include <signal.h>

/* to use fork */
#include <unistd.h>

/* other necessary headers */
#include <stdlib.h> 
#include <stdio.h> 
#include <string.h>

//#include <pthread.h> // ben sonradan ekledim

/* The ftok() function returns a key based on path and id that is usable in subsequent calls to semget() and shmget()
#define KEYSEM ftok(strcat(get_current_dir_name(), argv[0]) ,1)
#define KEYSEM2 ftok(strcat(get_current_dir_name(), argv[0]), 2)
#define KEYSHM ftok(strcat(get_current_dir_name(), argv[0]), 3)
*/
 
#define KEYSEM 1000
#define KEYSEM2 1200
#define KEYSHM 1500


/* semaphore increment operation */
void sem_signal (int semid, int val){
    struct sembuf semaphore ;
    semaphore.sem_num = 0;
    semaphore.sem_op = val;
    semaphore.sem_flg = 1;    /* relative: add sem op to value */
    semop(semid, &semaphore, 1) ;
}


/* semaphore decrement operation */
void sem_waitt(int semid, int val){
    struct sembuf semaphore ;
    semaphore.sem_num = 0;
    semaphore.sem_op = (-1 * val ) ;
    semaphore.sem_flg = 1;    /* relative: add sem op to value */
    semop(semid , &semaphore , 1) ;
}
    

/* signal-handling function */
void mysignal(int signum){
    printf("Received signal with num = %d.\n", signum);
}


void mysigset (int num){
    struct sigaction mysigaction;
    mysigaction.sa_handler=(void *)mysignal ;
    /* using signal-catching function identified by sa_handler */
    mysigaction.sa_flags=0;
    /* sigaction system call is used to change the action taken by a process on receipt of a specific signal (specified with num) */
    sigaction(num, &mysigaction, NULL) ;
}



int main(int argc, const char * argv[]) {
    mysigset (12) ;             /* signal handler with num=12 */
    int shmid = 0;              /* shared memory id */
    int *globalcp = NULL;       /* shared memory area */
    int localInt;              /* a locally defined integer */
    int termSem = 0, lock = 0;  /* semaphore ids */
    int f = 0;                     /* return value of fork () */
    int child[2];               /* child process ids */
    int i, myOrder = 0;        /* order of the running child process */
    
    /* creating 2 child processes */
    for (i=0; i<2; i++){
        f = fork() ;
        if (f == -1){
            printf("FORK error ....\n");
            exit(1);
        }
        if (f == 0) break ;
        child[i] = f ;
    }
    
    if (f != 0){
        /* creating a semaphore for synchronization(value=0) between parent and its children */
        termSem = semget(KEYSEM2, 1, 0700|IPC_CREAT);
        semctl(termSem, 0, SETVAL, 0);
        
        /* creating a semaphore for mutual exclusion ( value=1) between child processes */
        lock = semget(KEYSEM, 1, 0700|IPC_CREAT);
        semctl (lock, 0, SETVAL, 1);
        
        /* creating a shared memory area with the size of an int */
        shmid = shmget(KEYSHM, sizeof(int), 0700|IPC_CREAT);
        
        /* attaching the shared memory segment identified by shmid to the address space of the calling process (parent) */
        globalcp = (int *)shmat(shmid, 0, 0);
        
        
        /* initializing the value in the shared memory as 0 */
        *globalcp = 0;
        
        /* detaching the shared memory segment from the address space of the calling process (parent) */
        shmdt(globalcp);
        sleep(2); /* waiting for 2 seconds */
            
        /* sending the signal 12 to start child processes */
        printf("Parent has created resources. \n");
        printf("Now, it will start the child processes. \n");
        
        for (i = 0; i < 2; i++)
            kill(child[i], 12);
        
        /* decreasing semaphore value by 2(wait for all children) */
        sem_waitt(termSem ,2) ;
        printf("All child processes have finished their jobs.\n");
        
        /* removing the created semaphores and shared memory */
        semctl(termSem, 0, IPC_RMID, 0);
        semctl(lock, 0, IPC_RMID, 0);
        shmctl(shmid, IPC_RMID, 0);
        
        /* parent process is exiting */
        exit (0) ;
    }
    else{
        /* to show which child process is running */
        myOrder = i;
        
        /* wait until receiving a signal (kill signal) */ 
        pause();
        printf("child %d is starting ....\n", myOrder);
        
        /* returning the semaphore ids for KEYSEM and KEYSEM2 */
        lock = semget(KEYSEM, 1, 0);
        termSem = semget(KEYSEM2, 1, 0);
        
        /* returning the shared memory id associated with KEYSHM */
        shmid = shmget(KEYSHM, sizeof(int), 0);
        
        /* attaching the shared memory segment identified by shmid to the address space of the calling process ( child ) */
        globalcp = (int *) shmat(shmid, 0, 0);
        
        for (i = 0; i < 5; i++){
            /* waiting for the semaphore with id=lock to enter the critical section */
            sem_waitt(lock , 1);
            printf("child %d: Found the value: %d, i:%d\n", myOrder, *globalcp, i);
            
            /* updating the value of the shared memory segment */
            localInt = *globalcp ;
            
            sleep(1);
            
            /* waiting for a second */
            localInt += i;
            *globalcp = localInt;
            
            printf("child %d: Made the value: %d, i:%d\n", myOrder, *globalcp, i);
            
            /* making the critical section available again */
            sem_signal(lock ,1);
            
            sleep (1) ; /* waiting for a second */
        }
        
        
        /* detaching the shared memory segment from the address space of the calling process ( child ) */
        shmdt(globalcp);
        
        /* increase semaphore by 1(synchronization with parent) */
        sem_signal(termSem, 1);
        exit (0); /* child process is exiting */
    }

    return 0;
}
