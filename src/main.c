#define _BSD_SOURCE
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>
#include <time.h>
#include <sys/mman.h>

char *source_path;
char *target_path;
int sleep_time;
int check_directory();
DIR *opendir();
void copy();

int main (int argc, char *argv[]) {
	if(argc <= 2){
	        printf("Not enough\n");
	        return EXIT_FAILURE;
	}
	source_path = argv[1];
	target_path = argv[2];
	if(check_directory(source_path) != 0){
		printf("benc boi\n");
		return EXIT_FAILURE;
	}
	if(check_directory(target_path) != 0){
		printf("benc boi\n");
		return EXIT_FAILURE;
	}
	copy(source_path, target_path);
	return EXIT_SUCCESS;
}

int check_directory (const char *d)
{
    DIR *dirptr;

    if (access ( d, F_OK ) != -1 ) {
        // file exists
        if ((dirptr = opendir (d)) != NULL) {
            closedir (dirptr); /* d exists and is a directory */
        } else {
            return -2; /* d exists but is not a directory */
        }
    } else {
        return -1;     /* d does not exist */
    }

    return 0;
}

void copy(char *source_path, char *target_path, int size)
{
	char *buf = (char *)malloc(size);
    int file_in, file_out;
    int read_in, read_out;
    file_in = open(source_path, O_RDONLY);
    file_out = open(target_path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if(file_in == -1 || file_out==-1)
    {
        syslog(LOG_ERR, "The file did not open correctly!");
        exit(EXIT_FAILURE);
    }

    while((read_in = read(file_in, buf, sizeof(buf)))>0)
    {
    	read_out = write(file_out, buf, (ssize_t) read_in);
        if(read_out != read_in)
        {
            perror("Error");
            exit(EXIT_FAILURE);
        }
    }
    close(file_in);
    close(file_out);
    //change_parameters(source_path, target_path);
    syslog(LOG_INFO, "Successfully copied file %s", source_path);
}

void change_parameters(char *source_path, char *target_path)
{
    struct utimbuf new;
    new.actime = 0;
    new.modtime = get_time(source_path);
    if(utime(target_path, &new) != 0)
    {
        syslog(LOG_ERR, "Error related to modification date!");
        exit(EXIT_FAILURE);
    }
    mode_t old = get_chmod(source_path);
    if(chmod(target_path, old)!=0)
    {
        syslog(LOG_ERR, "File permission setting error!!");
        exit(EXIT_FAILURE);
    }
}
