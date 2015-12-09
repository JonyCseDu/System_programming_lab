#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#define PRINTLOG

inline int isDir(char name[]){
    //printf("ENTERED for %s\n", name);
    DIR *dir = opendir(name);
    if(dir == NULL) return 0;
    else{
        closedir(dir);
        return 1;
    }
}

/**
status
0   ->  ok
-1  ->  arg != 2
-2  ->  error in opening file or dir
-3  ->  error in getting file stat

*/

void printmsg(int x){
    printf("--LOG MESSAGE--\n");
    if(x == 65280) printf("something error occured in its child\n");
    else printf("successfully finished\n", x);
}

int main(int argc, char* argv[]){
    /// first create a child for printing list of files
    printf("%s:\n", argv[1]);
	int pid = fork();
    if ( pid == 0 ) {
//        char *argx[3] = { "./my_ls_l", argv[1], NULL};
//        char *env[3] = {"./my_ls_l", argv[1], NULL};
//        execve( "./my_ls_l", argx, env );
        execl( "./my_ls_l", "my_ls_l", argv[1], NULL);
    }
    else{
        int status;
        waitpid(-1, &status, 0); /* wait for child to exit */
        //printf("-->%d\n", (int)status);
        #ifdef PRINTLOG
            printmsg(status);
        #endif // PRINTLOG
    }
    putchar('\n');

    /// now iterate for directory
    DIR *dir = opendir(argv[1]);
    if(dir == NULL) exit(-2); /// dir can't open

    struct dirent *file;
    //struct dirent *dir_list[1000];
    char str[100], ara[150][150];
    int in = 0;
    while((file = readdir(dir)) != NULL){
        strcpy(str, argv[1]);
        strcat(str, file->d_name);
        strcat(str, "/");
        //strcpy(ara[in], str);

        if(file->d_name[0] != '.' && isDir(str)) {
            //printf("%s\n", str);
            int pid = fork();
            if ( pid == 0 ) {
                execl( "./my_ls_R", "my_ls_R", str, NULL);
            }
            else{
                int status;
                waitpid(-1, &status, 0); /* wait for child to exit */
                //printf("-->%d\n", status);
                #ifdef PRINTLOG
                    printmsg(status);
                #endif // PRINTLOG
            }
        }
    }
    closedir(dir);

    return 0;
}
