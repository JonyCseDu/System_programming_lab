#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#define MSGSZ     128


typedef struct msgbuf {
    long    mtype;
    char    mtext[MSGSZ];
    int cnt;
} message_buf;

void main()
{
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

    char str[MSGSZ];
    while(1){
        printf("message type : ");
        scanf("%ld", &sbuf.mtype);
        gets(str);
        printf("message body : ");
        gets(str);

        if(strcmp(str, "exit")==0) break;
        strcpy(sbuf.mtext, str);
        buf_length = strlen(sbuf.mtext) + 1 ;
        if (msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT) < 0) {
            printf ("%d, %d, %s, %d\n", msqid, sbuf.mtype, sbuf.mtext, buf_length);
            perror("msgsnd");
            exit(1);
        }
        else printf("Message: \"%s\" Sent\n", sbuf.mtext);
    };

    return 0;
}
