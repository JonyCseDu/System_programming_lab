#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <bits/stdc++.h>
using namespace std;

#define DB(x) cerr << #x << " = " << x << endl;

struct node{
    int pid;
    int port;
    int fd;
    node(int x, int y, int z){
        pid = x;
        port = y;
        fd = z;
    }
    bool operator < (const node &p) const{
        return pid < p.pid;
    }
};
vector<node> vec;
vector<string> ring_node;

void error(char * str){
    perror(str);
    exit(1);
}


/// sorts && generates ring info
void process_nodes(){
    sort(vec.begin(), vec.end());
    char buffer[100];
    int n;
    //sleep(1);
    for(int i=0; i<vec.size(); i++){
        /// read pid from ring nodes response
        bzero(buffer, sizeof(buffer));
        n = recv(vec[i].fd, buffer, 100, 0);
        if (n < 0) error("ERROR reading from socket");
        vec[i].pid = atoi(buffer);

        /// concate pid, predecessor and successor
        int be = (i-1+vec.size()) % vec.size(), af = (i+1+vec.size()) % vec.size();
        bzero(buffer, sizeof(buffer));
        sprintf(buffer, "%d %d %d", vec[i].port, vec[be].port, vec[af].port);

        /// store result
        ring_node.push_back(buffer);
    }
}

/// sends ring info to each node
void form_ring(){
    int n;
    for(int i=0; i<vec.size(); i++){
        n = send(vec[i].fd, ring_node[i].c_str(), ring_node[i].size()+1, 0);
        if (n < 0) error("ERROR writting to socket");
    }
}

void bind_socket(int &sockfd, int portno){

    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(serv_addr.sin_zero, '\0', sizeof serv_addr.sin_zero);

    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)  error("ERROR on binding");
}

int main( int argc, char *argv[] ) {
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in cli_addr;
    int n, pid;

    /* First call to socket() function */
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    /// bind socket
    bind_socket(sockfd, 5001);


    /* Now start listening for the clients, here
        * process will go in sleep mode and will wait
        * for the incoming connection
    */

    n = listen(sockfd, 5);
    if(n<0) error("listening error");

    clilen = sizeof(cli_addr);

    /// get ring number
    int n_of_nodes;
    printf("NUMBER OF RING NODES : ");
    scanf("%d", &n_of_nodes);

    /// create child ring nodes
    for(int i=0; i<n_of_nodes; i++){
        pid = fork();
        if(pid < 0) error("ERROR on fork");
        if(pid == 0){ /// child process
            close(sockfd);
            execl("./ringnode", "ringnode", NULL);
            return 0;
        }
    }

    /// accept ring sockets and store
    while (n_of_nodes--) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) error("ERROR on accept");
        vec.push_back(node(0, ntohs(cli_addr.sin_port), newsockfd)); /// pid will be read
    } /* end of while */

    process_nodes(); /// sorts && generates ring info
    form_ring();/// sends ring info to each node

    /// inject empty string
    sleep(1);
    n = send(vec[0].fd, "START", 6, 0);
    if(n<0) error("sending to socket");

    /// now close all ring socket
    for(int i=0; i<vec.size(); i++){
        close(vec[i].fd);
    }
    //sleep(1);
    close(sockfd);
    //return 0;
}
