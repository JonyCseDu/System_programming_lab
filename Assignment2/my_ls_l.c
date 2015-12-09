#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <stdint.h>
#include <fcntl.h>

/**
status
0   ->  ok
-1  ->  arg != 2
-2  ->  error in opening file
-3  ->  error in getting file stat

*/

/**
        struct stat {
               dev_t     st_dev;     // ID of device containing file
               ino_t     st_ino;     // inode number
               mode_t    st_mode;    // protection
               nlink_t   st_nlink;   // number of hard links
               uid_t     st_uid;     // user ID of owner
               gid_t     st_gid;     // group ID of owner
               dev_t     st_rdev;    // device ID (if special file)
               off_t     st_size;    // total size, in bytes
               blksize_t st_blksize; // blocksize for filesystem I/O
               blkcnt_t  st_blocks;  // number of 512B blocks allocated
               time_t    st_atime;   // time of last access
               time_t    st_mtime;   // time of last modification
               time_t    st_ctime;   // time of last status change
           };

*/

void print_mode(const struct stat fileStat){
    printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
}

int main(int argc, char* argv[]){
    //int i;
    //printf("number of arg %d\n", argc);
    //for(i=0; i<argc; i++) printf("--> %s\n", argv[i]);
    DIR *dir = opendir(argv[1]);
    if(dir == NULL) exit(-2); /// dir can't open

    struct dirent *file;
    while((file = readdir(dir)) != NULL){
        if(file->d_name[0] != '.'){
            ///getting filestat for the file
            struct stat filestat;
            if(stat(file->d_name, &filestat) == -1){ /// getting file status in filestat
                exit(-3);
            }

            /// printing mode and no of link
            print_mode(filestat);
            printf(" %d", filestat.st_nlink);

            /// printing user and grp name and size
            struct passwd *usr = getpwuid(filestat.st_uid);
            struct group  *grp = getgrgid(filestat.st_gid);
            printf(" %s", usr->pw_name);
            printf(" %s", grp->gr_name);
            printf(" %5d", (int)filestat.st_size);

            /// printing date-time, and file_name
            char datestring[256];
            struct tm time;
            localtime_r(&filestat.st_mtime, &time); /// time_t -> tm
            /* Get localized date string. */
            strftime(datestring, sizeof(datestring), "%F %T", &time); /// tm -> datestring(char *)

            printf(" %s %s\n", datestring, file->d_name);

        }
    }
    putchar('\n');
    closedir(dir);

    return 0;
}
