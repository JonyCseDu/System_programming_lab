#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>


int main(int argc, char* argv[]){
    //int i;
    //printf("number of arg %d\n", argc);
    //for(i=0; i<argc; i++) printf("--> %s\n", argv[i]);
    DIR *dir = opendir(argv[1]);
    if(dir == NULL) return -1; /// dir can't open

    struct dirent *file;
    while((file = readdir(dir)) != NULL){
        if(file->d_name[0] != '.' && file->d_name[strlen(file->d_name)-1] != '~') printf("%s ", file->d_name);
    }
    putchar('\n');
    closedir(dir);

    return 0;
}
