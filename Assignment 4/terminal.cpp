#include <bits/stdc++.h>
#include <unistd.h>
#include <fcntl.h>
using namespace std;


int main(){
    string str;
    //char *env[] = {"grep", "ter", NULL};
    //execvp(env[0], env);
    //execlp("ls", "ls", ".", NULL);
    //cout << "OK" << endl;
    while(1){
        getline(cin, str);
        if(str == "exit") return 0;
        //cout << str << endl;
        pid_t pid = fork();
        if(pid == 0){
            execl("handler", "handler", str.c_str(), NULL);
            perror("exec error");
        }
    }
    return 0;
}

