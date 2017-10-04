//
//  main.c
//  OShw1
//
//  Created by Tuğba Özkal on 1.03.2017.
//  Copyright © 2017 Tuğba Özkal. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

int main(void){
    /*
     
    // program stackoverflow
     
    int i;
    
    for (i=0;i<3;i++){
        fork();
        printf("[%d] [%d] i=%d\n", getppid(), getpid(), i);
    }
    
    printf("[%d] [%d] hi\n", getppid(), getpid());

    */



    // program hw1
    
    int sonuc = 1;
    
    for (int i = 1; i <= 3; i++){
        if (sonuc == 0){
            printf("Cocuk proses %d, pid = %d, ppid = %d \n",i, getpid(), getppid());
        }
        else{
            sonuc = fork();
            printf("Anne proses (i=%d), pid = %d \n",i, getpid());
        }
    }

    
    /*
    // program 2
     
    printf("\nMaster is working: PID = %d \n", getpid() );
    int f = fork();
    if (f == 0){
        printf("\nThis is child. PID = %d \n", getpid() );
        execlp("./execSlave", "./execSlave", "test1", "test2", (char*)NULL );
        printf("\n %s \n", strerror(errno));
    }
    else{
        wait(NULL);
        exit(0);
    }
    return 0;
    
    // program 1 
     
    int f = fork() ;
    if(f == -1){ // fork is not successful
        printf ("Error\n") ;
        exit(1);
    }
    else if (f == 0){ // child process
        printf("\tChild: Process ID: %d \n",getpid());
        // waiting for 10 seconds
        sleep (10) ;
        printf("\tChild: Parent Process ID: %d \n",getppid());
    }
    else{ // parent process
        printf("Parent: Process ID: %d \n", getpid());
        printf("Parent: Child Process ID: %d \n", f);
        printf("Parent: Parent Process ID: %d \n", getppid()); // waiting until child process has exited
        wait (NULL) ;
        printf("Parent: Terminating... \n");
        exit (0) ;
    }
    return 0;
    */
}
