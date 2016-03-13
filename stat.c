#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include <sys/types.h>
#include <pwd.h>
#include <grp.h>


int main(int argc, char* argv[]){

	struct stat fileStat;

	//statPrint(argv[1], &fileStat);

	stat(argv[1], &fileStat); //Get the files info

	printf("File: %s\n", argv[1]); //Char
	printf("Size: %ld   Blocks: %ld   IO-Block: %ld\n"
	, fileStat.st_size, fileStat.st_blocks, fileStat.st_blksize); //Long int
	printf("Device: 0x%lx   Inode: %ld   Links %ld \n"
		,fileStat.st_dev, fileStat.st_ino, fileStat.st_nlink);

	//Match the uid and gid of the file with the ones on the system.
	//http://stackoverflow.com/questions/7328327/how-to-get-files-owner-name-in-linux-using-c
    struct passwd *pw = getpwuid(fileStat.st_uid);
	struct group  *gr = getgrgid(fileStat.st_gid);

	//http://codewiki.wikidot.com/c:system-calls:stat
	printf("Access: (%u/", fileStat.st_mode); 	
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

    printf(")   UDI: (%d/%s)   GID: (%d/%s)\n"
    	, fileStat.st_uid, (*pw).pw_name, fileStat.st_gid, (*gr).gr_name);
    

 	//http://en.cppreference.com/w/c/chrono/timespec_get
    char buff[100];
    strftime(buff, sizeof buff, "%D %T", gmtime(&fileStat.st_atim.tv_sec));
    printf("Access: %s.%09ld UTC\n", buff, fileStat.st_atim.tv_nsec);

    strftime(buff, sizeof buff, "%D %T", gmtime(&fileStat.st_mtim.tv_sec));
    printf("Modify: %s.%09ld UTC\n", buff, fileStat.st_mtim.tv_nsec);

    strftime(buff, sizeof buff, "%D %T", gmtime(&fileStat.st_ctim.tv_sec));
    printf("Change: %s.%09ld UTC\n", buff, fileStat.st_ctim.tv_nsec);

	exit(EXIT_SUCCESS);
}