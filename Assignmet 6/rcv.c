#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include   <time.h>

#define MSGSZ     128

/**
struct msqid_ds {
    struct ipc_perm msg_perm;     // Ownership and permissions
    time_t          msg_stime;    // Time of last msgsnd(2)
    time_t          msg_rtime;    // Time of last msgrcv(2)
    time_t          msg_ctime;    // Time of last change
    unsigned long   __msg_cbytes; // Current number of bytes in queue (nonstandard)
 ->   msgqnum_t       msg_qnum;     // Current number of messages in queue
    msglen_t        msg_qbytes;   // Maximum number of bytes allowed in queue
    pid_t           msg_lspid;    // PID of last msgsnd(2)
    pid_t           msg_lrpid;    // PID of last msgrcv(2)
};
*/

typedef struct msgbuf {
    long    mtype;
    char    mtext[MSGSZ];
} message_buf;

int msqid;
message_buf  rbuf;

char * get(long t, long n, int rmv){
    struct msqid_ds buf;
    //msgctl(msqid, cmd, buf);
    if(msgctl(msqid, IPC_STAT, &buf) == -1){
        perror("msgctl error");
        exit(1);
    }

    int sz = buf.msg_qnum, i, cnt = 0;
    printf("size : %d\n", sz);
    char str[MSGSZ];

    for(i=1; i<=sz; i++){
        if (msgrcv(msqid, &rbuf, MSGSZ, 0, 0) < 0) {
            perror("msgrcv");
            exit(1);
        }
        printf("%ld %s\n", rbuf.mtype, rbuf.mtext);
        if(t == 0 || t == rbuf.mtype){
            cnt++;
            if(cnt == n){
                strcpy(str, rbuf.mtext);
            }
        }
        if(cnt == n && rmv){
            printf("removed msg : %s\n", rbuf.mtext);
        }
        else {
            int buf_length = strlen(rbuf.mtext) + 1 ;
            msgsnd(msqid, &rbuf, buf_length, IPC_NOWAIT);
        }
    }
    if(!rmv) return str;
    else return NULL;
}


int main()
{
    key_t key = 1234;
    if ((msqid = msgget(key, IPC_CREAT | 0666)) < 0) {
        perror("msgget");
        exit(1);
    }

    char str[MSGSZ];
    long t, n;
    while(1){
        scanf("%s", str);
        if(strcmp(str, "exit") == 0) break;
        scanf("%ld %ld", &t, &n);
        if(strcmp(str, "get") == 0){
            strcpy(str, get(t, n, 0));
            printf("recieved : %s\n", str);
        }
        else{
            get(0, n, 1);
        }
    }
    //msgctl(msqid, IPC_RMID, NULL);
    return 0;
}
