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
#define KEYN ftok(strcat(get_current_dir_name(), argv[0]), 3)
#define KEYM ftok(strcat(get_current_dir_name(), argv[0]), 4)
#define KEYX ftok(strcat(get_current_dir_name(), argv[0]), 5)
#define KEYY ftok(strcat(get_current_dir_name(), argv[0]), 6)
#define KEYSHM1 ftok(strcat(get_current_dir_name(), argv[0]), 3)
*/

#define KEYSEM 2000
#define KEYSEM2 2200
#define KEYN 2500
#define KEYM 2501
#define KEYX 2502
#define KEYY 2503
#define KEYSHM1 2504

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
    semop(semid, &semaphore , 1) ;
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
    int sayac1, sayac2;
    int size;
    
    int termSem = 0, lock = 0;
    
    mysigset (12) ;             // signal handler with num = 12
    
    int child[2];
    int ord;
    int f = 0;                  // fork() sonucu
    int processOrder = 0;
    int id_n = 0, id_m = 0, id_x = 0, id_y = 0;
    int shmid = 0, shmid2 = 0, shmid3 = 0;
    
    int KEYSHM2 = 0, KEYSHM3 = 0;
    
    int *A_arr = NULL;
    int *B_arr = NULL;
    int *C_arr = NULL;
    
    id_n = shmget(KEYN, sizeof(int), 0700|IPC_CREAT);
    n = (int*)shmat(id_n, 0, 0);
    
    id_m = shmget(KEYM, sizeof(int), 0700|IPC_CREAT);
    m = (int*)shmat(id_m, 0, 0);
    
    id_x = shmget(KEYM, sizeof(int), 0700|IPC_CREAT);
    x = (int*)shmat(id_x, 0, 0);
    
    id_y = shmget(KEYM, sizeof(int), 0700|IPC_CREAT);
    y = (int*)shmat(id_y, 0, 0);
    
    
    printf("A dizisinin eleman sayisini giriniz: ");
    scanf("%d", n);
    
    shmid = shmget(KEYSHM1, (*n)*sizeof(int), 0700|IPC_CREAT);
    A_arr = (int *)shmat(shmid, 0, 0);
    
    srand(time(NULL));
    for(int j = 0; j < *n; j++){
        A_arr[j] = rand() % 100;    // uretilecek sayiyi 100 ile sinirladim. odevde istenilen bir durum degildi aslinda ama cok buyuk sayilar uretilmesini istemedim.
    }
    
    printf("Olusturulan A dizisi: A={");
    for(int j = 0; j < *n; j++){
        printf("%d,", A_arr[j]);
    }
    printf("}\n");
    
    printf("A dizisinin baslangic adresi: %p\n\n", A_arr);
    
    printf("M sayisini giriniz: ");
    scanf("%d", m);
    
    const int N = *n;
    const int deger = *m;
    
    struct shmid_ds shmbuffer ;
    
    shmctl (shmid, IPC_STAT, &shmbuffer );
    size = shmbuffer.shm_segsz;
    printf("A segment size = %d\n", size);
    
    
    printf("\n");
    
    shmdt(A_arr);
    shmdt(n);
    
    sleep(2); // waiting for 2 seconds
    
    // Bir anne prosesten 2 cocuk proses olusturdu.
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
        shmctl(shmid, IPC_RMID, 0);
        shmctl(shmid2, IPC_RMID, 0);
        shmctl(shmid3, IPC_RMID, 0);
        shmctl(id_x, IPC_RMID, 0);
        shmctl(id_y, IPC_RMID, 0);
        shmctl(id_m, IPC_RMID, 0);
        shmctl(id_n, IPC_RMID, 0);
        
        exit(0);
    }
    else{
        processOrder = ord;

        pause();

        sleep(1);

        lock = semget(KEYSEM, 1, 0);
        termSem = semget(KEYSEM2, 1, 0);
        
        shmid = shmget(KEYSHM1, N*sizeof(int), 0);
        A_arr = (int*)shmat(shmid, 0, 0);
        
        id_n = shmget(KEYN, sizeof(int), 0);
        n = (int*)shmat(id_n, 0, 0);
        
        int bir=0, iki=0;

        for (int j = 0; j < N; j++){
            sem_waitt(lock , 1);
            temp = A_arr[j];
            if(temp > deger){
                iki++;
            }
            else if(temp <= deger){
                bir++;
            }
            sleep(1);
            sem_signal(lock, 1);
            sleep(1);
        }
        
        *x = bir;
        *y = iki;

        KEYSHM2 = KEYSHM1 + N*sizeof(int);
        shmid2 = shmget(KEYSHM2, bir*sizeof(int), 0700|IPC_CREAT);
        B_arr = (int *)shmat(shmid2, 0, 0);
        
        
        shmctl (shmid2, IPC_STAT, &shmbuffer );
        size = shmbuffer.shm_segsz;
        printf("B segment size = %d\n", size);

        
        KEYSHM3 = KEYSHM2 + bir*sizeof(int);
        shmid3 = shmget(KEYSHM3, iki*sizeof(int), 0700|IPC_CREAT);
        C_arr = (int *)shmat(shmid3, 0, 0);
        
        shmctl (shmid3, IPC_STAT, &shmbuffer );
        size = shmbuffer.shm_segsz;
        printf("C segment size = %d\n", size);

        sayac1 = -1;
        sayac2 = -1;
        for(int j = 0; j < *n; j++){
            sem_waitt(lock , 1);
            temp = A_arr[j];
            if(temp <= deger){
                sayac1++;
                B_arr[sayac1] = temp;
            }
            if(temp > deger){
                sayac2++;
                C_arr[sayac2] = temp;
            }
            sleep(1);
            sem_signal(lock, 1);
            sleep(1);
        }
        
        
        printf("Cocuk 1: <= M olan eleman sayisi = %d\n", bir);
        printf("Cocuk 1: B dizisinin baslangic adresi: %p\n", B_arr);
        printf("Cocuk 1: B dizisi: B={");
        for(int j = 0; j < bir; j++){
            printf("%d,", B_arr[j]);
        }
        printf("}\n\n");
        
        printf("Cocuk 2: > M olan eleman sayisi = %d\n", iki);
        printf("Cocuk 2: C dizisinin baslangic adresi: %p\n", C_arr);
        printf("Cocuk 2: C dizisi: C={");
        for(int j = 0; j < iki; j++){
            printf("%d,", C_arr[j]);
        }
        printf("}\n\n");
        
        
        sem_signal(termSem, 1);
        
        sleep(1);
        
        shmdt(A_arr);
        shmdt(B_arr);
        shmdt(C_arr);
        shmdt(x);
        shmdt(y);
        
        
        exit(0);
            
    }
    return 0;
}
