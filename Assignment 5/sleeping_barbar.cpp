#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

/// uses 2 semapores
#define CUSTOMERS 0
#define MUTEX 1

#define CHAIRS 5


struct my_sems{
    int sem_id;
    struct sembuf *semaphore;
    my_sems(int sem_key){
        sem_id=semget(sem_key, 2, IPC_CREAT|0600);

        initSem(CUSTOMERS, 0);
        initSem(MUTEX, 1);
    }
    void up(int sem_num, struct sembuf *semaphore) {
        semaphore->sem_num=sem_num;
        semaphore->sem_op=1;
        semaphore->sem_flg=0;
        semop(sem_id, semaphore, 1);
    }
    void down(int sem_num, struct sembuf *semaphore) {
        semaphore->sem_num=sem_num;
        semaphore->sem_op=-1;
        semaphore->sem_flg=0;
        semop(sem_id, semaphore, 1);
    }
    void initSem(int sem_num, int val) {
        union semnum {
            int val;
            struct semid_ds *buf;
            unsigned short *array;
        }argument;
        argument.val=val;
        semctl(sem_id, sem_num, SETVAL, argument);
    }
};



int main() {

    int sem_key=1111, shm_key=2222;
    int shm_id;
    int *waiting;
    struct sembuf semaphore;
    int cnt=1;
    my_sems mysem(sem_key);

    printf("ok\n");
    shm_id=shmget(shm_key, sizeof(int), IPC_CREAT|0600); /// returns the identifier of the System V shared memory segment associated with the value of the argument key
    waiting=(int *)shmat(shm_id, NULL, 0); /// attaches the System V shared memory segment identified by shmid to the address space of the calling process
    *waiting=0;

    printf("ok\n");
    printf("There are %d chairs.\n",CHAIRS);
    int pid = fork();
    if(pid == -1){
        printf("error occoured\n");
        return -1;
    }
    if(pid) {
    /* The barber part. */
        while(true) {
            mysem.down(CUSTOMERS, &semaphore);
            mysem.down(MUTEX, &semaphore);

            *waiting=*waiting-1;
            //printf("%d\n", *waiting);
            printf("The barber is now cutting hair.\n");
            mysem.up(MUTEX, &semaphore);
            sleep(5);   /* Slowly cut the hair */
        }
    }

    else {
    /* The customer part. */
        while(true) {
            sleep(rand()%5);   /* Customers come in fast */
            mysem.down(MUTEX, &semaphore);
            if(*waiting < CHAIRS) {
                printf("Customer %d is seated.\n",cnt++);
                *waiting=*waiting+1;
                mysem.up(CUSTOMERS, &semaphore);
                mysem.up(MUTEX, &semaphore);
            }
            else {
                printf("Customer %d left the shop.\n",cnt++);
                mysem.up(MUTEX, &semaphore);
            }
        }
    }

}
