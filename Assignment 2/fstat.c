#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <utime.h>

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
    //printf("ok1\n");
    if(argc != 2) return -1;
    //printf("ok2\n");
    /// open file
    int file = open(argv[1], O_RDONLY);
    if(file == -1) return -2;
    //printf("ok3\n");
    /// get file stat

    struct stat filestat;
    if(fstat(file, &filestat) == -1) return -3;
    //if(fstat(file, &filestat) < 0) return -3;
    //printf("%d\n", fstat(file, filestat));
    /// printing info
    printf("Information for %s\n",argv[1]);
    printf("---------------------------\n");
    printf("ID of device containing file:\t%d\n", filestat.st_dev);
    printf("inode number:\t\t\t%d\n", filestat.st_ino);
    printf("protection:\t\t\t"); print_mode(filestat) ; putchar('\n');
    printf("number of hard links:\t\t%d\n", filestat.st_nlink);

    printf("user ID of owner:\t\t%d\n", filestat.st_uid);
    printf("group ID of owner:\t\t%d\n", filestat.st_gid);

    printf("File size:\t\t\t%lld bytes\n",(long long) filestat.st_size);
    printf("Last status change:\t\t%s", ctime(&filestat.st_ctime));
    printf("Last file access:\t\t%s", ctime(&filestat.st_atime));
    printf("time of last modification:\t%s", ctime(&filestat.st_mtime));

    printf("NOW CHANGING FILE MODIFICATION TIME ...\n");

    struct utimbuf new_times; /// update time buffered structure
    new_times.actime = filestat.st_atime;
    new_times.modtime = 10000;
    utime(argv[1], &new_times);

    //filestat.st_atime = 10000; /// get current cal time

    //printf("time of last modification:\t%s", ctime(&filestat.st_mtime));


    return 0;
}
