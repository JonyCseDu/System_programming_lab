/* The sleeping barber problem */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#define CUSTOMERS 0
#define BARBERS 1
#define MUTEX 2
#define CHAIRS 5

void up(int sem_id,int sem_num,struct sembuf *semaphore) {
  semaphore->sem_num=sem_num;
  semaphore->sem_op=1;
  semaphore->sem_flg=0;
  semop(sem_id,semaphore,1);
}

void down(int sem_id,int sem_num,struct sembuf *semaphore) {
  semaphore->sem_num=sem_num;
  semaphore->sem_op=-1;
  semaphore->sem_flg=0;
  semop(sem_id,semaphore,1);
}

void initSem(int sem_id, int sem_num, int val) {
  union semnum {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
  }argument;
  argument.val=val;
  semctl(sem_id, sem_num, SETVAL, argument);
}

int main() {

  int sem_key=1111, shm_key=2222;
  int sem_id, shm_id;
  int *waiting;
  struct sembuf semaphore;
  int count=1;

  shm_id=shmget(shm_key, sizeof(int), IPC_CREAT|0666); /// returns the identifier of the System V shared memory segment associated with the value of the argument key
  sem_id=semget(sem_key, 3, IPC_CREAT|0666);

  waiting=shmat(shm_id, NULL, 0); /// attaches the System V shared memory segment identified by shmid to the address space of the calling process
  *waiting=0;

  initSem(sem_id, CUSTOMERS, 0);
  initSem(sem_id, BARBERS, 0);
  initSem(sem_id, MUTEX, 1);

  printf("There are %d chairs.\n",CHAIRS);

  if(fork()) {

    /* The barber part. */

    while(1) {

      down(sem_id,CUSTOMERS,&semaphore);

      down(sem_id,MUTEX,&semaphore);

      *waiting=*waiting-1;

      up(sem_id,BARBERS,&semaphore);

      up(sem_id,MUTEX,&semaphore);

      printf("The barber is now cutting hair.\n");

      sleep(6);   /* Slowly cut the hair */

    }

  }

  else {

    /* The customer part. */

    while(1) {

      sleep(1);   /* Customers come in fast */

      down(sem_id,MUTEX,&semaphore);

      if(*waiting < CHAIRS) {

      printf("Customer %d is seated.\n",count++);

      *waiting=*waiting+1;

      up(sem_id,CUSTOMERS,&semaphore);

      up(sem_id,MUTEX,&semaphore);

      }

      else {

      printf("Customer %d left the shop.\n",count++);

      up(sem_id,MUTEX,&semaphore);

      }

    }

  }

}
