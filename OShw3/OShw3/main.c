//
//  main.c
//  OShw3
//
//  Created by Tuğba Özkal on 8.05.2017.
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

#define KEYSHM1 1000
#define KEYSHM2 1001
#define KEYSHM3 1002
#define KEYSHM4 1003
#define KEYSEM_CALISAN_OKUYUCU_YOK 1100
#define KEYSEM_CALISAN_YAZICI_YOK 1200
#define KEYSEM_BEKLEYEN_OKUYUCU_YOK 1300
#define KEYSEM_BEKLEYEN_YAZICI_YOK 1400
#define KEYSEM_LASTSEM 1500


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

int OKUYUCU_SAYISI = 5;
int YAZICI_SAYISI = 1;

int okuyucu_sayaci;
int yazici_sayaci;

int main(int argc, char* argv[]){
    
    struct istek * Okuyucular, * Yazicilar;
    struct istek * okuyucu_kuyrugu, * yazici_kuyrugu;
    
    char elcevap = 'E';
    int f = 1;
    int cocuk[20];
    int proses_sayisi = 0;
    int shmid_1 = 0, shmid_2 = 0, shmid_3 = 0, shmid_4 = 0;
    int CALISAN_OKUYUCU_YOK, CALISAN_YAZICI_YOK, BEKLEYEN_OKUYUCU_YOK, BEKLEYEN_YAZICI_YOK, lastsem;
    okuyucu_sayaci = 0;
    yazici_sayaci = 0;
    
    mysigset (12) ;             // signal handler with num = 12
    
    
    shmid_1 = shmget(KEYSHM1, sizeof(struct istek)*OKUYUCU_SAYISI, 0700 | IPC_CREAT);
    Okuyucular = (struct istek *)shmat(shmid_1, 0, 0);
    
    shmid_2 = shmget(KEYSHM2, sizeof(struct istek)*YAZICI_SAYISI, 0700 | IPC_CREAT);
    Yazicilar = (struct istek *)shmat(shmid_2, 0, 0);
    
    shmid_3 = shmget(KEYSHM3, sizeof(struct istek), 0700 | IPC_CREAT);
    yazici_kuyrugu = (struct istek *)shmat(shmid_3, 0, 0);
    
    shmid_4 = shmget(KEYSHM4, sizeof(struct istek), 0700 | IPC_CREAT);
    okuyucu_kuyrugu = (struct istek *)shmat(shmid_4, 0, 0);
    
    
    okuyucu_kuyrugu = NULL;
    yazici_kuyrugu = NULL;
    
    while(elcevap == 'E' || elcevap == 'e'){
        struct istek * yeni_istek = malloc(sizeof(struct istek));
        
        
        printf("Proses tipini, sirasini ve calisma suresini saniye cinsinden giriniz (Ornegin; R 1 10, W 2 20):");
        scanf("%c", &yeni_istek->type);
        scanf("%d", &yeni_istek->ord);
        scanf("%d", &yeni_istek->duration);
        proses_sayisi++;
        
        f = fork();
        if (f == -1){
            printf("fork() error ...\n");
            exit(1);
        }
        yeni_istek->pid = f;
        
        struct istek * traverse;
        switch (yeni_istek->type){
            case 'r': case 'R':
                traverse = okuyucu_kuyrugu;
                while(traverse){
                    traverse = traverse->next;
                }
                traverse = yeni_istek;
                printf("R%d kuyruga alindi.", yeni_istek->ord);
                pause();
                break;
                
            case 'w': case 'W':
                traverse = yazici_kuyrugu;
                while(traverse){
                    traverse = traverse->next;
                }
                traverse = yeni_istek;
                printf("W%d kuyruga alindi.", yeni_istek->ord);
                pause();
                break;
                
            default:
                printf("Hatali giris yaptiniz...");
                break;
        }
        
        
        
        printf("Devam etmek istiyor musunuz?(E/H)\n");
        scanf("%c", &elcevap);
    }

    
    
    
    if(f != 0){
        
        CALISAN_OKUYUCU_YOK = semget(KEYSEM_CALISAN_OKUYUCU_YOK, 1, 0700 | IPC_CREAT);
        semctl(CALISAN_OKUYUCU_YOK, 0, SETVAL, OKUYUCU_SAYISI);
        
        CALISAN_YAZICI_YOK = semget(KEYSEM_CALISAN_YAZICI_YOK, 1, 0700 | IPC_CREAT);
        semctl(CALISAN_YAZICI_YOK, 0, SETVAL, YAZICI_SAYISI);
        
        BEKLEYEN_OKUYUCU_YOK = semget(KEYSEM_BEKLEYEN_OKUYUCU_YOK, 1, 0700 | IPC_CREAT);
        semctl(BEKLEYEN_OKUYUCU_YOK, 0, SETVAL, 1);
        
        BEKLEYEN_YAZICI_YOK = semget(KEYSEM_BEKLEYEN_YAZICI_YOK, 1, 0700 | IPC_CREAT);
        semctl(BEKLEYEN_YAZICI_YOK, 0, SETVAL, 1);
        
        lastsem = semget(KEYSEM_LASTSEM, 1, 0700 | IPC_CREAT);
        semctl(lastsem, 0, SETVAL, 1);
        
        

        sleep(2);
        
        sem_waitt(lastsem, proses_sayisi);
        
        printf("Tum prosesler isini tamamladi.\n");
        
        semctl(BEKLEYEN_OKUYUCU_YOK, 0, IPC_RMID, 0);
        semctl(BEKLEYEN_YAZICI_YOK, 0, IPC_RMID, 0);
        semctl(CALISAN_OKUYUCU_YOK, 0, IPC_RMID, 0);
        semctl(CALISAN_YAZICI_YOK, 0, IPC_RMID, 0);
        semctl(lastsem, 0, IPC_RMID, 0);
        shmctl(shmid_1, IPC_RMID, 0);
        shmctl(shmid_2, IPC_RMID, 0);
        shmctl(shmid_3, IPC_RMID, 0);
        shmctl(shmid_4, IPC_RMID, 0);
        
        exit(0);

    }   // end if(f != 0)
    else{
        pause();
        
        shmid_1 = shmget(KEYSHM1, sizeof(struct istek)*OKUYUCU_SAYISI, 0);
        shmid_2 = shmget(KEYSHM2, sizeof(struct istek)*YAZICI_SAYISI, 0);
        shmid_3 = shmget(KEYSHM3, sizeof(struct istek), 0);
        shmid_4 = shmget(KEYSHM4, sizeof(struct istek), 0);
        
        Okuyucular = (struct istek *)shmat(shmid_1, 0, 0);
        Yazicilar = (struct istek *)shmat(shmid_2, 0, 0);
        okuyucu_kuyrugu = (struct istek *)shmat(shmid_3, 0, 0);
        yazici_kuyrugu = (struct istek *)shmat(shmid_4, 0, 0);
        
        CALISAN_OKUYUCU_YOK = semget(KEYSEM_CALISAN_OKUYUCU_YOK, 1, 0);
        CALISAN_YAZICI_YOK = semget(KEYSEM_CALISAN_YAZICI_YOK, 1, 0);
        
        BEKLEYEN_OKUYUCU_YOK = semget(KEYSEM_BEKLEYEN_OKUYUCU_YOK, 1, 0);
        BEKLEYEN_YAZICI_YOK = semget(KEYSEM_BEKLEYEN_YAZICI_YOK, 1, 0);
        
        lastsem = semget(KEYSEM_LASTSEM, 1, 0);
        
        okuyucu_sayaci = 0;
        
        struct istek * tr;
        tr = Okuyucular;
        struct istek myprocess;
        
        while(tr){
            tr = &Okuyucular[okuyucu_sayaci];
            if(tr == NULL) break;
            if(tr->pid == getpid()){
                myprocess = *tr;
            }
            okuyucu_sayaci++;
            tr = tr->next;
        }
        
               
               
        if(myprocess.type == 'r' || myprocess.type == 'R'){
                   
            if(okuyucu_sayaci < 5 && okuyucu_kuyrugu != NULL){
                       
                sem_waitt(CALISAN_YAZICI_YOK, 1);
                       
                Okuyucular[okuyucu_sayaci] = *okuyucu_kuyrugu;
                okuyucu_sayaci++;
                printf("Calisan okuyucu sayisi %d. R%d calismaya basladi.", okuyucu_sayaci, okuyucu_kuyrugu->ord);
                okuyucu_kuyrugu = okuyucu_kuyrugu->next;
                sleep(Okuyucular[okuyucu_sayaci-1].duration);
                okuyucu_sayaci--;
                printf("R%d bitti.", Okuyucular[okuyucu_sayaci].ord);
                
                if(okuyucu_sayaci == 0){
                    sem_signal(CALISAN_OKUYUCU_YOK, 1);
                }
                
            }
                   
            if(okuyucu_kuyrugu == NULL){
                sem_signal(BEKLEYEN_OKUYUCU_YOK, 1);
            }
                   
            
        }
        else {
                   
            if(yazici_kuyrugu != NULL && okuyucu_kuyrugu == NULL){
                       
                sem_waitt(CALISAN_OKUYUCU_YOK, 1);
                sem_waitt(CALISAN_YAZICI_YOK, 1);
                sem_waitt(BEKLEYEN_OKUYUCU_YOK, 1);
                       
                Yazicilar[yazici_sayaci] = yazici_kuyrugu[0];
                yazici_sayaci++;
                printf("Calisan yazici sayisi %d. W%d calismaya basladi.", yazici_sayaci, yazici_kuyrugu->ord);
                yazici_kuyrugu = yazici_kuyrugu->next;
                sleep(Yazicilar[0].duration);
                yazici_sayaci--;
                printf("W%d bitti.", Yazicilar[yazici_sayaci].ord);
                sem_signal(CALISAN_YAZICI_YOK, 1);
            }
                   
            if(yazici_kuyrugu == NULL){
                sem_signal(BEKLEYEN_YAZICI_YOK, 1);
            }
                   
        }
        
        sem_signal(lastsem, 1);
        
        shmdt(okuyucu_kuyrugu);
        shmdt(yazici_kuyrugu);
        shmdt(Okuyucular);
        shmdt(Yazicilar);
        
        exit(0);
    }   // end else of if(f != 0)
    
    return 0;
}
