#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <bits/stdc++.h>
#include <arpa/inet.h>
#define MAX 1000
using namespace std;
#define DB(x) cerr << #x << " = " << x << endl;

void error(char * str){
    perror(str);
    exit(1);
}

int SetTimer(struct timeval &tv, time_t sec)
{
    gettimeofday(&tv,NULL);
    tv.tv_sec+=sec;
    return 1;
}

int CheckTimer(struct timeval &tv)
{
    struct timeval ctv;
    gettimeofday(&ctv,NULL);

    if( (ctv.tv_sec > tv.tv_sec) )
    {
        gettimeofday(&tv,NULL);
        return 1;
    }
    else return 0;
}

void connect_with_server(int &sockfd, int portno){
    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(portno);
    memset(serv_addr.sin_zero, '\0', sizeof serv_addr.sin_zero);
    int ret = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (ret < 0)  error("ERROR connecting");
}

void bind_socket(int &sockfd, int portno){
    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    memset(serv_addr.sin_zero, '\0', sizeof serv_addr.sin_zero);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)  error("ERROR on binding");
}

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    char buffer[MAX+1];
    char pid[10];
    sprintf(pid, " %d", getpid());

    /// set up timer
    struct timeval tv;
    SetTimer(tv, 5); /// set up timer for 1 min or 60s

    /// Create a socket point
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)  error("ERROR opening socket");

    /// connect with server
    connect_with_server(sockfd, 5001);

    /// send pid to master
    bzero(buffer, sizeof(buffer));
    sprintf(buffer, "%d\n", getpid());
    n = send(sockfd, buffer, strlen(buffer), 0);
    if (n < 0) error("ERROR writing to socket");

    /// read pid, predecessor and successor
    bzero(buffer, sizeof(buffer));
    n = recv(sockfd, buffer, 255, 0);
    if (n < 0)  error("ERROR reading from socket");
    printf("client reading : %s\n", buffer);

    /// store pid, predecessor and successor
    int myport, predecessorPort, successorPort;
    sscanf(buffer, "%d %d %d", &myport, &predecessorPort, &successorPort);

    /// initialize necessory variables for ring processing
    int successorFd, predecessorFd;
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);

    /// try to accept "START" string
    bzero(buffer, sizeof(buffer));
    n = recv(sockfd, buffer, 255, 0);
    if (n < 0)  error("ERROR reading from socket");

    /// communication with master finished
    close(sockfd);

    /// bind and listen to socket
    successorFd = socket(AF_INET, SOCK_STREAM, 0);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bind_socket(sockfd, myport);
    n = listen(sockfd, 2);
    if(n<0) error("listening error");

    /// synchornization point between successor and predecessor
    if(strcmp(buffer, "START") == 0){ /// first connect then accept
        sleep(1);
        connect_with_server(successorFd, successorPort);
        predecessorFd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (predecessorFd < 0) error("ERROR on accept");

        /// initiate ring chain reaction
        n = send(successorFd, pid, strlen(pid), 0);
        if(n<0) error("chain initialization failed");
    }
    else{ /// first accept then connect
        predecessorFd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (predecessorFd < 0) error("ERROR on accept");
        connect_with_server(successorFd, successorPort);
    }

    /// communication between ring nodes
    while(true){
        /// read first
        bzero(buffer, sizeof(buffer));
        n = recv(predecessorFd, buffer, MAX, 0);
        if (n < 0) error("ERROR reading from socket");

        /// check timer and kill interrupt
        if(strcmp(buffer, "KILL") == 0){
            //DB("KILLING NOW");
            break;
        }
        if(CheckTimer(tv)==1){ /// timer finished
            /// send kill message to successor
            n = send(successorFd, "KILL", 5, 0); /// ignore failure most possible cause timer
            sleep(1); /// wait for all ring nodes termination
            /// now print
            printf("%s\n", buffer);
            break;
        }

        else{ /// no timer interrupt
            strcat(buffer, pid);
            n = send(successorFd, buffer, MAX, 0);
        }
    }
    close(sockfd);
    close(successorFd);
    close(predecessorFd);
    printf("finished child\n");

    return 0;
}
