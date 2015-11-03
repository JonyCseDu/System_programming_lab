#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;
int main(int argc, int *argv[]){
    int n, lo, hi;

    printf("NUMBER OF PROCESS : ");
    scanf("%d", &n);
    printf("Range : a b\n");
    scanf("%d %d", &lo, &hi);

    if(lo>hi) swap(lo, hi);
    int x = (hi-lo)/n;

    int fd[2];
    if(pipe(fd) == -1){
        perror("pipe error");
        exit(1);
    }

    for(int i=0; i<n; i++){
        char a[7], b[7], des[7];
        sprintf(a, "%7d", max(2, lo+i*x));
        sprintf(b, "%7d", min(lo+(i+1)*x-1, hi));
        sprintf(des, "%7d", fd[1]);

        int pid = fork();
        if(pid == -1){
            perror("fork error");
            exit(1);
        }
        else if(pid == 0){
            close(fd[0]);
            execl("./prime", "prime", a, b, des, NULL);
        }
    }

    close(fd[1]);
    sleep(1e-100);
    kill(-getpid(), SIGQUIT);

    int num;
    char buf[10];
    vector<int> vec;
    while(read(fd[0], buf, 7) > 0){
        //cout << "-->" << buf << endl;
        num = atoi(buf);
        vec.push_back(num);
    }

    sort(vec.begin(), vec.end());
    for(int i=0; i<vec.size(); i++){
        printf("%d ", vec[i]);
    }
    putchar('\n');
    //cout << "end" << endl;
    return 0;
}
