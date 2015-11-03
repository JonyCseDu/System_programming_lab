#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char *argv[]){
    //cout << "-->" << argc << endl;
    if(argc != 4){
        perror("argument error");
        exit(1);
    }
    int lo = atoi(argv[1]), hi = atoi(argv[2]), des = atoi(argv[3]);
    //cout << lo << " " << hi << endl;

    for(int i=lo; i<=hi; i++){
        int j=0, rt = sqrt(i);
        for(j=2; j<=rt; j++){
            if(i%j==0) break;
        }
        //cout << "ok" << endl;
        if(j>rt) {
            char buf[7];
            sprintf(buf, "%7d", i);
            write(des, buf, 7);
        }
    }
    exit(0);
}
