#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#define LOCK 0
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include   <time.h>
using namespace std;

struct my_sems{
    int sem_id;
    struct sembuf *semaphore;
    my_sems(int sem_key){
        sem_id=semget(sem_key, 1, IPC_CREAT|0666);
        initSem(LOCK, 1);
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

#define MSGSZ 100
typedef struct node {
    long    mtype;
    char    mtext[MSGSZ];
} message_buf;


vector< pair<int, int> > vec;
int main(){
    int sem_key=1111, shm_key=2222;
    int shm_id;
    int *waiting;
    struct sembuf semaphore;
    int cnt=1;
    my_sems mysem(sem_key);

    int msqid;
    int msgflg = IPC_CREAT | 0666;
    key_t key = 1234;

    message_buf sbuf;
    size_t buf_length;

    if ((msqid = msgget(key, msgflg )) < 0) {
        perror("msgget");
        exit(1);
    }
    else printf("msgget: msgget succeeded: msqid = %d\n", msqid);

    //printf("ok\n");
    shm_id=shmget(shm_key, sizeof(int), IPC_CREAT|0600); /// returns the identifier of the System V shared memory segment associated with the value of the argument key
    waiting=(int *)shmat(shm_id, NULL, 0); /// attaches the System V shared memory segment identified by shmid to the address space of the calling process
    *waiting=0;

    int i=0, p1, p2, p3, p4, p5;
    int fd[2];
    pipe(fd);
    p1 = fork();

    if(p1 && p1 != 0){ ///    1
        int msqid;
        int msgflg = IPC_CREAT | 0666;
        key_t key = 1234;

        message_buf sbuf;
        size_t buf_length;

        if ((msqid = msgget(key, msgflg )) < 0) {
            perror("msgget");
            exit(1);
        }

        while(*waiting < 50){
            //printf("child1\n");

            //printf("%d\n", *waiting);
            if(*waiting < 50){
                mysem.up(LOCK, &semaphore);
                *waiting = *waiting + 1;
                int msqid;
                int msgflg = IPC_CREAT | 0666;
                key_t key = 1234;

                message_buf sbuf;
                size_t buf_length;

                if ((msqid = msgget(key, msgflg )) < 0) {
                    perror("msgget");
                    exit(1);
                }
                //else printf("msgget: msgget succeeded: msqid = %d\n", msqid);

                char str[MSGSZ];
                sbuf.mtype = 1;
                strcpy(sbuf.mtext, "0");
                sbuf.mtext[0] += *waiting;
                buf_length = strlen(sbuf.mtext) + 1 ;
                if (msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT) < 0) {
                    printf ("%d, %d, %s, %d\n", msqid, sbuf.mtype, sbuf.mtext, buf_length);
                    perror("msgsnd");
                    exit(1);
                }//else printf("Message: \"%s\" Sent\n", sbuf.mtext);
                sleep(1);
                mysem.down(LOCK, &semaphore);
            }
        }
        return 0;
    }



    p1 = fork();
    if(p1 && p1 != 0){
        //printf("child2");
        int msqid;
        int msgflg = IPC_CREAT | 0666;
        key_t key = 1234;

        message_buf sbuf;
        size_t buf_length;

        if ((msqid = msgget(key, msgflg )) < 0) {
            perror("msgget");
            exit(1);
        }
        //printf("%d\n", *waiting);
        while(*waiting < 50){
            //printf("child1\n");

            //printf("%d\n", *waiting);
            printf("child2");
            if(*waiting < 50){
                mysem.down(LOCK, &semaphore);
                *waiting = *waiting + 1;
                int msqid;
                int msgflg = IPC_CREAT | 0666;
                key_t key = 1234;

                message_buf sbuf;
                size_t buf_length;

                if ((msqid = msgget(key, msgflg )) < 0) {
                    perror("msgget");
                    exit(1);
                }
                //else printf("msgget: msgget succeeded: msqid = %d\n", msqid);

                char str[MSGSZ];
                sbuf.mtype = 2;
                strcpy(sbuf.mtext, "0");
                sbuf.mtext[0] += *waiting;
                buf_length = strlen(sbuf.mtext) + 1 ;
                if (msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT) < 0) {
                    printf ("%d, %d, %s, %d\n", msqid, sbuf.mtype, sbuf.mtext, buf_length);
                    perror("msgsnd");
                    exit(1);
                }//else printf("Message: \"%s\" Sent\n", sbuf.mtext);
                mysem.up(LOCK, &semaphore);
                sleep(1);
            }
        }
        return 0;
    }

    p1 = fork();
    if(p1 && p1 != 0){
        //printf("child2");
        int msqid;
        int msgflg = IPC_CREAT | 0666;
        key_t key = 1234;

        message_buf sbuf;
        size_t buf_length;

        if ((msqid = msgget(key, msgflg )) < 0) {
            perror("msgget");
            exit(1);
        }
        //printf("%d\n", *waiting);
        while(*waiting < 50){
            //printf("child1\n");

            //printf("%d\n", *waiting);
            printf("child2");
            if(*waiting < 50){
                mysem.down(LOCK, &semaphore);
                *waiting = *waiting + 1;
                int msqid;
                int msgflg = IPC_CREAT | 0666;
                key_t key = 1234;

                message_buf sbuf;
                size_t buf_length;

                if ((msqid = msgget(key, msgflg )) < 0) {
                    perror("msgget");
                    exit(1);
                }
                //else printf("msgget: msgget succeeded: msqid = %d\n", msqid);

                char str[MSGSZ];
                sbuf.mtype = 3;
                strcpy(sbuf.mtext, "0");
                sbuf.mtext[0] += *waiting;
                buf_length = strlen(sbuf.mtext) + 1 ;
                if (msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT) < 0) {
                    printf ("%d, %d, %s, %d\n", msqid, sbuf.mtype, sbuf.mtext, buf_length);
                    perror("msgsnd");
                    exit(1);
                }//else printf("Message: \"%s\" Sent\n", sbuf.mtext);
                mysem.up(LOCK, &semaphore);
                sleep(1);
            }
        }
        return 0;
    }

    p1 = fork();
    if(p1 && p1 != 0){
        //printf("child2");
        int msqid;
        int msgflg = IPC_CREAT | 0666;
        key_t key = 1234;

        message_buf sbuf;
        size_t buf_length;

        if ((msqid = msgget(key, msgflg )) < 0) {
            perror("msgget");
            exit(1);
        }
        //printf("%d\n", *waiting);
        while(*waiting < 50){
            //printf("child1\n");

            //printf("%d\n", *waiting);
            printf("child2");
            if(*waiting < 50){
                mysem.down(LOCK, &semaphore);
                *waiting = *waiting + 1;
                int msqid;
                int msgflg = IPC_CREAT | 0666;
                key_t key = 1234;

                message_buf sbuf;
                size_t buf_length;

                if ((msqid = msgget(key, msgflg )) < 0) {
                    perror("msgget");
                    exit(1);
                }
                //else printf("msgget: msgget succeeded: msqid = %d\n", msqid);

                char str[MSGSZ];
                sbuf.mtype = 4;
                strcpy(sbuf.mtext, "0");
                sbuf.mtext[0] += *waiting;
                buf_length = strlen(sbuf.mtext) + 1 ;
                if (msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT) < 0) {
                    printf ("%d, %d, %s, %d\n", msqid, sbuf.mtype, sbuf.mtext, buf_length);
                    perror("msgsnd");
                    exit(1);
                }//else printf("Message: \"%s\" Sent\n", sbuf.mtext);
                mysem.up(LOCK, &semaphore);
                sleep(1);
            }
        }
        return 0;
    }

    p1 = fork();
    if(p1 && p1 != 0){
        //printf("child2");
        int msqid;
        int msgflg = IPC_CREAT | 0666;
        key_t key = 1234;

        message_buf sbuf;
        size_t buf_length;

        if ((msqid = msgget(key, msgflg )) < 0) {
            perror("msgget");
            exit(1);
        }
        //printf("%d\n", *waiting);
        while(*waiting < 50){
            //printf("child1\n");

            //printf("%d\n", *waiting);
            printf("child2");
            if(*waiting < 50){
                mysem.down(LOCK, &semaphore);
                *waiting = *waiting + 1;
                int msqid;
                int msgflg = IPC_CREAT | 0666;
                key_t key = 1234;

                message_buf sbuf;
                size_t buf_length;

                if ((msqid = msgget(key, msgflg )) < 0) {
                    perror("msgget");
                    exit(1);
                }
                //else printf("msgget: msgget succeeded: msqid = %d\n", msqid);

                char str[MSGSZ];
                sbuf.mtype = 5;
                strcpy(sbuf.mtext, "0");
                sbuf.mtext[0] += *waiting;
                buf_length = strlen(sbuf.mtext) + 1 ;
                if (msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT) < 0) {
                    printf ("%d, %d, %s, %d\n", msqid, sbuf.mtype, sbuf.mtext, buf_length);
                    perror("msgsnd");
                    exit(1);
                }//else printf("Message: \"%s\" Sent\n", sbuf.mtext);
                mysem.up(LOCK, &semaphore);
                sleep(1);
            }
        }
        return 0;
    }






    else{
        waitpid(-1, 0, 0);
        struct msqid_ds buf;
        //msgctl(msqid, cmd, buf);
        if(msgctl(msqid, IPC_STAT, &buf) == -1){
            perror("msgctl error");
            exit(1);
        }

        int sz = buf.msg_qnum, i, cnt = 0;
        printf("size : %d\n", sz);
        char str[MSGSZ];

        message_buf  rbuf;
        for(i=1; i<=sz; i++){
            if (msgrcv(msqid, &rbuf, MSGSZ, 0, 0) < 0) {
                perror("msgrcv");
                exit(1);
            }
            printf("%ld %d\n", rbuf.mtype, rbuf.mtext[0] - '0');
        }
    }
    //pid = fork();
    return 0;
}
