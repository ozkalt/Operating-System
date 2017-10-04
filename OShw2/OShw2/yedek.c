//
//  main.c
//  OShw2
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
#include <sys/stat.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>


/*
#define KEYSEM ftok(strcat(get_current_dir_name(), argv[0]) ,1)
#define KEYSEM2 ftok(strcat(get_current_dir_name(), argv[0]), 2)
#define KEYSHM ftok(strcat(get_current_dir_name(), argv[0]), 3)
*/


#define KEYSEM 2000
#define KEYSEM2 2200


// semaphore increment operation
void sem_signal (int semid, int val){
    struct sembuf semaphore ;
    semaphore.sem_num = 0;
    semaphore.sem_op = val;
    semaphore.sem_flg = 1;    // relative: add sem op to value
    semop(semid, &semaphore, 1) ;
}


// semaphore decrement operation
void sem_waitt(int semid, int val){
    struct sembuf semaphore ;
    semaphore.sem_num = 0;
    semaphore.sem_op = (-1 * val ) ;
    semaphore.sem_flg = 1;    /* relative: add sem op to value */
    semop(semid , &semaphore , 1) ;
}

void mysignal(int signum){

}

void mysigset(int num){
    struct sigaction mysigaction;
    mysigaction.sa_handler = (void*) mysignal;
    mysigaction.sa_flags = 0;
    sigaction(num, &mysigaction, NULL);
}


int main(int argc, char* argv[]){
    
    int *n, *m;
    int *x = NULL;
    int *y = NULL;
    int temp = 0;
    int sayac;
    
    int termSem = 0, lock = 0;
    
    mysigset (12) ;             // signal handler with num = 12
    
    int child[2];
    int ord;
    int f = 0;                  // fork() sonucu
    int processOrder = 0;
    int id_n = 0, id_m = 0, id_x = 0, id_y = 0;
    int shmid = 0, shmid2 = 0, shmid3 = 0;
    
    int KEYN = 2500;
    int KEYM, KEYX, KEYY;
    int KEYSHM1 = 0, KEYSHM2 = 0, KEYSHM3 = 0;
    
    int *A_arr = NULL;
    int *B_arr = NULL;
    int *C_arr = NULL;
    
    id_n = shmget(KEYN, sizeof(int), 0700|IPC_CREAT);
    n = (int*)shmat(id_n, 0, 0);
    
    KEYM = KEYN + (int)sizeof(int);
    
    id_m = shmget(KEYM, sizeof(int), 0700|IPC_CREAT);
    m = (int*)shmat(id_m, 0, 0);
    
    KEYX = KEYM + (int)sizeof(int);
    
    id_x = shmget(KEYM, sizeof(int), 0700|IPC_CREAT);
    x = (int*)shmat(id_x, 0, 0);
    
    KEYY = KEYX + (int)sizeof(int);
    
    id_y = shmget(KEYM, sizeof(int), 0700|IPC_CREAT);
    y = (int*)shmat(id_y, 0, 0);
    
    
    
    printf("A dizisinin eleman sayisini giriniz: ");
    scanf("%d", n);
    
    KEYSHM1 = KEYY + (int)sizeof(int);
    
    shmid = shmget(KEYSHM1, *n*sizeof(int), 0700|IPC_CREAT);
    A_arr = (int *)shmat(shmid, 0, 0);
    
    *A_arr = 0;
    
    srand(time(NULL));
    for(int j = 0; j < *n; j++){
        A_arr[j] = rand() % 100;    // uretilecek sayiyi 100 ile sinirladim. odevde istenilen bir durum degildi aslinda ama cok buyuk sayilar uretilmesini istemedim.
    }
    
    printf("Olusturulan A dizisi: A={");
    for(int j = 0; j < *n; j++){
        printf("%d,", A_arr[j]);
    }
    printf("}\n");
    
    printf("n baslangic adresi: %p\n", n);      // optional
    printf("M baslangic adresi: %p\n", m);      // optional
    printf("x dizisinin baslangic adresi: %p\n", x);    // optional
    printf("y dizisinin baslangic adresi: %p\n", y);    // optional
    printf("A dizisinin baslangic adresi: %p\n\n", A_arr);
    
    printf("M sayisini giriniz: ");
    scanf("%d", m);
    
    const int deger = *m;
    
    printf("\n");
    
    shmdt(A_arr);
    sleep(2); // waiting for 2 seconds
    
    // Bir anne prosesten 2 cocuk proses olusturulur.
    for (ord = 0; ord < 2; ord++){
        f = fork();
        if (f == -1){
            printf("fork() error ....\n");
            exit(1);
        }
        if (f == 0)     break ;
        child[ord] = f;
    }
    
    for (int k = 0; k < 2; k++){
        kill(child[k], 12);
    }
    
    if(f != 0){
        
        termSem = semget(KEYSEM2, 1, 0700|IPC_CREAT);
        semctl(termSem, 0, SETVAL, 0);
        
        lock = semget(KEYSEM, 1, 0700|IPC_CREAT);
        semctl (lock, 0, SETVAL, 1);
        
        sem_waitt(termSem, 2);
        printf("Tum cocuk prosesler isini tamamladi.\n");
        
        semctl(termSem, 0, IPC_RMID, 0);
        semctl(lock, 0, IPC_RMID, 0);
        shmctl(shmid, IPC_RMID, 0);
    
        exit(0);
    }
    else{
        pause();

        processOrder = ord;

        sleep(1);

        lock = semget(KEYSEM, 1, 0);
        termSem = semget(KEYSEM2, 1, 0);
        
        
        shmid = shmget(KEYSHM1, *n*sizeof(int), 0);
        
        A_arr = (int *)shmat(shmid, 0, 0);
        
        
        /*
        *y = 0;
        for (int j = 0; j < *n; j++){
            temp = A_arr[j];
            if (temp > deger){
                (*y)++;
                printf("y = %d\t", *y);
            }
        }
        printf("\n");
        
        sem_waitt(lock , 1);
        KEYSHM3 = KEYSHM2 + *x*sizeof(int);
        shmid3 = shmget(KEYSHM3, *y*sizeof(int), 0700|IPC_CREAT);
        C_arr = (int *)shmat(shmid3, 0, 0);
        
        *C_arr = 0;
        
        sayac = -1;
        for(int j = 0; j < *n; j++){
            temp = A_arr[j];
            if(temp > deger){
                sayac++;
                C_arr[sayac] = temp;
            }
        }
        printf("\n");
        
        printf("Cocuk %d: > M olan eleman sayisi = %d\n", processOrder, *y);
        printf("Cocuk %d: C dizisinin baslangic adresi: %p\n", processOrder, C_arr);
        printf("Cocuk %d: C dizisi: C={", processOrder);
        for(int j = 0; j < *y; j++){
            printf("%d,", C_arr[j]);
        }
        printf("}\n\n");
        

        
        
        *x = 0;
        
        for(int j = 0; j < *n; j++){
            temp = A_arr[j];
            if(temp <= deger){
                (*x)++;
                printf("x = %d\t", *x);
            }
        }
        printf("\n");
        sem_signal(lock, 1);
        
        KEYSHM2 = KEYSHM1 + (*n)*(int)sizeof(int);
        
        shmid2 = shmget(KEYSHM2, (*x)*(int)sizeof(int), 0700|IPC_CREAT);
        B_arr = (int *)shmat(shmid2, 0, 0);
        
        *B_arr = 0;
        
        sayac = -1;
        for(int j = 0; j < *n; j++){
            temp = A_arr[j];
            if(temp <= deger){
                sayac++;
                B_arr[sayac] = temp;
            }
        }
        printf("\n");
        
        printf("Cocuk %d: <= M olan eleman sayisi = %d\n", processOrder, *x);
        printf("Cocuk %d: B dizisinin baslangic adresi: %p\n", processOrder, B_arr);
        printf("Cocuk %d: B dizisi: B={", processOrder);
        for(int j = 0; j < *x; j++){
            printf("%d,", B_arr[j]);
        }
        printf("}\n\n");
        
        */
    
        
        //sleep(1);
    
        
        if(processOrder == 0){
            *x = 0;
            for (int j = 0; j < *n; j++){
                temp = A_arr[j];
                if (temp <= deger){
                    (*x)++;
                }
            }
            
            sleep(1);
            
            KEYSHM2 = KEYSHM1 + *n*sizeof(int);
    
            shmid2 = shmget(KEYSHM2, *x*sizeof(int), 0700|IPC_CREAT);
            B_arr = (int *)shmat(shmid2, 0, 0);
            
            *B_arr = 0;
        
            sayac = -1;
            for(int j = 0; j < *n; j++){
                temp = A_arr[j];
                if(temp <= deger){
                    sayac++;
                    B_arr[sayac] = temp;
                }
            }
        
            printf("Cocuk %d: <= M olan eleman sayisi = %d\n", processOrder, *x);
            printf("Cocuk %d: B dizisinin baslangic adresi: %p\n", processOrder, B_arr);
            printf("Cocuk %d: B dizisi: B={", processOrder);
            for(int j = 0; j < *x; j++){
                printf("%d,", B_arr[j]);
            }
            printf("}\n\n");
            
            sleep(1);
            sem_signal(lock, 1);
        }
        
        sleep(1);
        
        sem_waitt(lock , 1);
        if(processOrder == 1){
            *y = 0;
            for (int j = 0; j < *n; j++){
                temp = A_arr[j];
                
                if (temp > deger){
                    (*y)++;
                }
            }
            
            KEYSHM3 = KEYSHM2 + *x*sizeof(int);
            
            shmid3 = shmget(KEYSHM3, *y*sizeof(int), 0700|IPC_CREAT);
            C_arr = (int *)shmat(shmid3, 0, 0);
            
            *C_arr = 0;
            
            sayac = -1;
            for(int j = 0; j < *n; j++){
                temp = A_arr[j];
                if(temp > deger){
                    sayac++;
                    C_arr[sayac] = temp;
                }
            }
            
            printf("Cocuk %d: > M olan eleman sayisi = %d\n", processOrder, *y);
            printf("Cocuk %d: C dizisinin baslangic adresi: %p\n", processOrder, C_arr);
            printf("Cocuk %d: C dizisi: C={", processOrder);
            for(int j = 0; j < *y; j++){
                printf("%d,", C_arr[j]);
            }
            printf("}\n\n");
        }
        
        sleep(1);

        
        sem_signal(termSem, 1);
        
        shmdt(A_arr);
        shmdt(B_arr);
        shmdt(C_arr);
        shmdt(x);
        shmdt(y);
        
        
        exit(0);
            
    }
    return 0;
}
