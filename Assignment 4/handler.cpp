
#include <bits/stdc++.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

#define DB(x) cerr << #x << " = " << x << endl;

vector<char *> vec;
void build_before(string str, int len){
    str = string(str.begin(), str.begin()+len);
    //DB(str);
    stringstream ss(str);
    bool first = true;
    vec.clear();

    while(ss >> str){
        //DB(str);
        char *tmp = (char *) malloc(str.size());
        strcpy(tmp, str.c_str());
        vec.push_back(tmp);
    }
    //cout << vec[0] << " -> " << vec[1] <<  " " << vec[2] << endl;
}

int main(int argc, char *argv[]){
    //DB(argv[1]);
    int len = strlen(argv[1]);
    for(int i=1; i<len; i++)
    {
        if(argv[1][i] == '|'){
            //cout << "Entered" << endl;
            int fd[2];
            pipe(fd); /// pipe point
            pid_t pid = fork(); /// fork point
            if(pid == -1) {
                perror("fork error");
                return 1;
            }
            if (pid == 0) {
                //cout << argv[1][i] << endl;
                dup2(fd[1], 1); /// writing to pipe instead of STDOUT
                build_before(argv[1], i);
                char **env = &vec[0];
                //cout << env[1] << " -> " << env[2] << endl;
                execvp(env[0], env);
                perror("child exec");
                return 1;
            }
            else{
                dup2(fd[0], 0); /// reading from pipe instead of STDIN
                close(fd[1]);
                waitpid(-1, 0, 0);
                string tmp(argv[1]+i+1, argv[1] + strlen(argv[1]) ); /// generating new string
                execl("handler", "handler", tmp.c_str(), NULL);
                perror("parent exec");
                return 1;
            }
            break;
        }
        else if(i == len-1){
            build_before(argv[1], strlen(argv[1]));
            char **env = &vec[0];
            //cout << "YES" << endl;
            execvp(env[0], env);
            perror("exec");
            return 1;
        }
    }
    waitpid(-1, 0, 0);
    //cout << "END" << endl;

    return 0;
}
