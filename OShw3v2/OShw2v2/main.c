//
//  main.c
//  OShw2v2
//
//  Created by Tuğba Özkal on 17.05.2017.
//  Copyright © 2017 Tuğba Özkal. All rights reserved.
//

#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <unistd.h>
#include <signal.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define KEYSHM 1000
#define KEYSHM2 1100
#define KEYSHM3 1200
#define KEYSEM_CALISAN_OKUYUCU_YOK 2100
#define KEYSEM_CALISAN_YAZICI_YOK 2200
#define KEYSEM_BEKLEYEN_OKUYUCU_YOK 2300
#define KEYSEM_BEKLEYEN_YAZICI_YOK 2400
#define KEYSEM_LASTSEM 2500



void sem_signal (int semid, int val){
    struct sembuf semaphore ;
    semaphore.sem_num = 0;
    semaphore.sem_op = val;
    semaphore.sem_flg = 1;
    semop(semid, &semaphore, 1) ;
}


void sem_waitt(int semid, int val){
    struct sembuf semaphore ;
    semaphore.sem_num = 0;
    semaphore.sem_op = (-1 * val ) ;
    semaphore.sem_flg = 1;
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

struct istek{
    int duration;
    char type;
    int ord;
    int pid;
    struct istek* next;
};


int main(int argc, char* argv[]){
    
    mysigset(12);
    
    int f = -1;
    int order;
    
    int CALISAN_OKUYUCU_YOK = 0, CALISAN_YAZICI_YOK = 0, BEKLEYEN_OKUYUCU_YOK = 0, BEKLEYEN_YAZICI_YOK = 0;
    int lastSem = 0;
    
    struct istek * calisanislem, * okuyucu_kuyrugu, * yazici_kuyrugu;
    int shmid = 0, shmid2 = 0, shmid3 = 0;

    
    
    if(f != 0){
        
        char elcevap = 'e';
        while(elcevap == 'e' || elcevap == 'E'){
            struct istek * yeni_istek = malloc(sizeof(struct istek));
            printf("Proses tipini ve calisma suresini saniye cinsinden giriniz (Ornegin; R 1 10, W 2 20):\n");
            scanf("%c", &yeni_istek->type);
            scanf("%d", &yeni_istek->ord);
            scanf("%d", &yeni_istek->duration);
            yeni_istek->next = NULL;
            
            
            struct istek * traverse;
            switch (yeni_istek->type){
                case 'r': case 'R':
                    traverse = okuyucu_kuyrugu;
                    while(traverse){
                        traverse = traverse->next;
                    }
                    traverse = yeni_istek;
                    printf("R%d kuyruga alindi.", yeni_istek->ord);
                    order = 0;
                    pause();
                    break;
                    
                case 'w': case 'W':
                    traverse = yazici_kuyrugu;
                    while(traverse){
                        traverse = traverse->next;
                    }
                    traverse = yeni_istek;
                    printf("W%d kuyruga alindi.", yeni_istek->ord);
                    order = 1;
                    pause();
                    break;
                    
                default:
                    printf("Hatali giris yaptiniz...");
                    break;
            }
            
            f = fork();
            if (f == -1){
                printf("fork() error ...\n");
                exit(1);
            }
            yeni_istek->pid = f;
            
            
            printf("Devam etmek istiyor musunuz?(E/H)\n");
            scanf("%c", &elcevap);
        }

        
        lastSem = semget(KEYSEM_LASTSEM, 1, 0700 | IPC_CREAT);
        semctl(lastSem, 0, SETVAL, 0);
        
        CALISAN_OKUYUCU_YOK = semget(KEYSEM_CALISAN_OKUYUCU_YOK, 1, 0700 | IPC_CREAT);
        semctl(CALISAN_OKUYUCU_YOK, 0, SETVAL, 1);
        
        CALISAN_YAZICI_YOK = semget(KEYSEM_CALISAN_YAZICI_YOK, 1, 0700 | IPC_CREAT);
        semctl(CALISAN_YAZICI_YOK, 0, SETVAL, 1);
        
        BEKLEYEN_OKUYUCU_YOK = semget(KEYSEM_BEKLEYEN_OKUYUCU_YOK, 1, 0700 | IPC_CREAT);
        semctl(BEKLEYEN_OKUYUCU_YOK, 0, SETVAL, 1);
        
        BEKLEYEN_YAZICI_YOK = semget(KEYSEM_BEKLEYEN_YAZICI_YOK, 1, 0700 | IPC_CREAT);
        semctl(BEKLEYEN_YAZICI_YOK, 0, SETVAL, 1);
        
        shmid = shmget(KEYSHM, sizeof (struct istek *), 0700|IPC_CREAT) ;
        calisanislem = (struct istek *)shmat(shmid, 0, 0);
        calisanislem = NULL;
        shmdt(calisanislem);
        
        shmid2 = shmget(KEYSHM2, sizeof (struct istek *), 0700|IPC_CREAT) ;
        okuyucu_kuyrugu = (struct istek *)shmat(shmid2, 0, 0);
        okuyucu_kuyrugu = NULL;
        shmdt(okuyucu_kuyrugu);
        
        shmid3 = shmget(KEYSHM3, sizeof (struct istek *), 0700|IPC_CREAT) ;
        yazici_kuyrugu = (struct istek *)shmat(shmid3, 0, 0);
        yazici_kuyrugu = NULL;
        shmdt(yazici_kuyrugu);

        
        sleep(2);
        
        sem_waitt(lastSem, 2);
        
        
        semctl(lastSem, 0, IPC_RMID, 0);
        semctl(BEKLEYEN_OKUYUCU_YOK, 0, IPC_RMID, 0);
        semctl(BEKLEYEN_YAZICI_YOK, 0, IPC_RMID, 0);
        semctl(CALISAN_OKUYUCU_YOK, 0, IPC_RMID, 0);
        semctl(CALISAN_YAZICI_YOK, 0, IPC_RMID, 0);
        shmctl(shmid, IPC_RMID, 0);
        shmctl(shmid2, IPC_RMID, 0);
        shmctl(shmid3, IPC_RMID, 0);
        
        exit(0);
        
    }
    else{
        
        printf("baba naber?\n");
        
        pause();
        
        lastSem = semget(KEYSEM_LASTSEM, 1, 0);

        CALISAN_OKUYUCU_YOK = semget(KEYSEM_CALISAN_OKUYUCU_YOK, 1, 0);
        CALISAN_YAZICI_YOK = semget(KEYSEM_CALISAN_YAZICI_YOK, 1, 0);
        
        BEKLEYEN_OKUYUCU_YOK = semget(KEYSEM_BEKLEYEN_OKUYUCU_YOK, 1, 0);
        BEKLEYEN_YAZICI_YOK = semget(KEYSEM_BEKLEYEN_YAZICI_YOK, 1, 0);
        
        shmid = shmget(KEYSHM, sizeof(struct istek *), 0);
        calisanislem = (struct istek *)shmat(shmid, 0, 0);
        
        shmid2 = shmget(KEYSHM2, sizeof(struct istek *), 0);
        okuyucu_kuyrugu = (struct istek *)shmat(shmid, 0, 0);
        
        shmid3 = shmget(KEYSHM3, sizeof(struct istek *), 0);
        yazici_kuyrugu = (struct istek *)shmat(shmid, 0, 0);

        
        if(order == 0){     // okuyucu
                
            sem_waitt(CALISAN_OKUYUCU_YOK, 1);
            sem_waitt(CALISAN_YAZICI_YOK, 1);
            
            printf("R%d calismaya basladi.", okuyucu_kuyrugu->ord);
            sleep(okuyucu_kuyrugu->duration);
            printf("R%d bitti.", okuyucu_kuyrugu->ord);
            okuyucu_kuyrugu = okuyucu_kuyrugu->next;
            
            sem_signal(CALISAN_OKUYUCU_YOK, 1);
                
            sleep(1);
                
            if(okuyucu_kuyrugu == NULL){
                sem_signal(BEKLEYEN_OKUYUCU_YOK, 1);
            }

        }
            
        sleep(1);
            
        if(order == 1){     // yazici
                
            sem_waitt(CALISAN_OKUYUCU_YOK, 1);
            sem_waitt(CALISAN_YAZICI_YOK, 1);
            sem_waitt(BEKLEYEN_OKUYUCU_YOK, 1);
                
            printf("W%d calismaya basladi.", yazici_kuyrugu->ord);
            sleep(yazici_kuyrugu->duration);
            printf("W%d bitti.", yazici_kuyrugu->ord);
            yazici_kuyrugu = yazici_kuyrugu->next;
            sem_signal(CALISAN_YAZICI_YOK, 1);
            
            sleep(1);
            
            if(yazici_kuyrugu == NULL){
                sem_signal(BEKLEYEN_YAZICI_YOK, 1);
            }
            
        }
        
        shmdt(calisanislem);
        shmdt(yazici_kuyrugu);
        shmdt(okuyucu_kuyrugu);
        
        sem_signal(lastSem, 1);
        
        exit(0);
    }
    
    return 0;
}



















