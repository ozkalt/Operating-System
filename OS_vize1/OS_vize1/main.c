//
//  main.c
//  OS_vize1
//
//  Created by Tuğba Özkal on 8.03.2017.
//  Copyright © 2017 Tuğba Özkal. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int t1;

int main (void) {
    int i, f;
    int t2, *t3;
    
    t1 = 1;

    for (i = 0; i < 2; i++) {
        f = fork();
        if (f == 0)
            break;
    }
    
    t2 = 0;
    
    if (f == 0) {
        t3 = (int *) malloc(sizeof(int));
        *t3 = 0;
        
        for (i = 0; i < 5; i++) {
            t1++;
            t2++;
            (*t3)++;
        }
        printf("By, Proses pid=%d t1=%d t2=%d t3=%d\n", getpid(), t1, t2, *t3);
        free(t3);
        exit(0);
    }
    else {
        t3 = (int *) malloc(sizeof(int));
        *t3 = 0;
        printf("Hi, Proses pid=%d t1=%d t2=%d t3=%d\n", getpid(), t1, t2, *t3);
        wait(NULL);
        wait(NULL);
        free(t3);
        exit(0);
    }
    return (0);
}
