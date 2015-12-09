#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

/**
chdir()


*/


inline int isDir(char name[]){
    //printf("ENTERED for %s\n", name);
    DIR *dir = opendir(name);
    if(dir == NULL) return 0;
    else{
        closedir(dir);
        return 1;
    }
}

int main(int argc, char* argv[]){
    /// first create a child for printing list of files
    printf("%s:\n", argv[1]);
	int pid = fork();
    if ( pid == 0 ) {
        char *argx[3] = { "./my_ls", argv[1], NULL };
        char *env[3] = {"./my_ls", argv[1], NULL};
        execve( "./my_ls", argx, env );
    }
    else{
        waitpid(pid,0,0); /* wait for child to exit */
    }
    putchar('\n');

    /// now iterate for directory
    DIR *dir = opendir(argv[1]);
    if(dir == NULL) return -1; /// dir can't open

    struct dirent *file;
    //struct dirent *dir_list[1000];
    char str[500], ara[550][550];
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
                //printf("trying\n");
                char *argx[3] = { "./my_ls_R", str, NULL };
                char *env[3] = {"./my_ls_R", str, NULL};
                execve( "./my_ls_R", argx, env );
            }
            else{
                waitpid(pid, 0, 0); /* wait for child to exit */
            }
        }
    }
    closedir(dir);

    return 0;
}
