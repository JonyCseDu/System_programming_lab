#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

/// waitpid(pid,0,0); /* wait for child to exit */

void print_error(int x){
    if(x == -1) printf("CAN'T OPEN FILE\n");
    if(x == -2) printf("ONLY SUPPORTED FUNCTIONALITY IS -R\n");
}

int main( int argc, char* argv[] ) {
    if(argc == 2){
        if(strcmp(argv[argc-1], "-R")){
            print_error(-2);
            return -2;
        }
        else{
            int pid = fork();
            if ( pid == 0 ) {
                char *argx[3] = { "./my_ls_R", "./", NULL };
                char *env[3] = {"./my_ls_R", "./", NULL};
                execve( "./my_ls_R", argx, env );
            }
            else{
                waitpid(pid,0,0); /* wait for child to exit */
            }
        }
    }
    else{
        int pid = fork();
        if ( pid == 0 ) {
            //printf("YES\n");
            char *argx[3] = { "./my_ls", "./", NULL };
            char *env[3] = {"./my_ls", "./", NULL};
            execve( "./my_ls_R", argx, env );
        }
        else{
            waitpid(pid,0,0); /* wait for child to exit */
        }
    }

	return 0;
}
