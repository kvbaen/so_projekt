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
#include <utime.h>
#include <stdint.h>
#include <stdbool.h>

void change_parameters();
char *source_path;
int size;
char *target_path;
int sleep_time;
DIR *opendir();
bool recursive = false;


int check_directory (const char *d)
{
    DIR *dirptr;

    if (access ( d, F_OK ) != -1 ) {
        // file exists
        if ((dirptr = opendir (d)) != NULL) {
            closedir (dirptr);
        } else {
            return -2; /* d exists but is not a directory */
        }
    } else {
        return -1;  /* d does not exist */
    }

    return 0; //directory
}

char *join_paths(char *directory_path, char *file_path){
	char *joined_paths = malloc(strlen(directory_path) + strlen(file_path) + 2);
	strcpy(joined_paths, directory_path);
	strcat(joined_paths, "/");
	strcat(joined_paths, file_path);
	//printf("jp na 100%s \n", joined_paths);
	return joined_paths;
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
    change_parameters(source_path, target_path);
    syslog(LOG_INFO, "Successfully copied file %s", source_path);
}

off_t get_size(char *source_path)
{
    struct stat size;
    if(stat(source_path, &size)==0)
    {
        return size.st_size;
    }
    return -1;
}

mode_t get_chmod(char *source_path)
{
    struct stat mod;
    if(stat(source_path, &mod)==-1)
    {
        syslog(LOG_ERR, "Blad pobrania chmod dla pliku %s", source_path);
        exit(EXIT_FAILURE);
    }
    return mod.st_mode;
}

time_t get_time(char* source_path)
{
    struct stat time;
    if(stat(source_path, &time) == -1)
    {
        syslog(LOG_ERR, "Error %s!", source_path);
        exit(EXIT_FAILURE);
    }
    return time.st_mtim.tv_sec;
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

void synchronize(char *source_path, char *target_path){
	DIR *d = opendir(source_path);
	struct dirent *dir;
	if(check_directory(source_path) == 0 && check_directory(target_path) == 0){
		while((dir = readdir(d))){
			if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0){
				continue;
			}
			char *joined_source_path = join_paths(source_path, dir->d_name);
			char *joined_target_path = join_paths(target_path, dir->d_name);
			if(check_directory(joined_source_path) == 0 && !recursive){
				continue;
			}
			if(check_directory(joined_source_path) == 0 ){
				mkdir(joined_target_path , get_chmod(joined_source_path));
				synchronize(joined_source_path, joined_target_path);
			}
			else{
				size = get_size(dir->d_name);
				copy(joined_source_path, joined_target_path, size);
			}
		}
	}
	else{
		exit(EXIT_FAILURE);
	}

}

void delete(char *source_path, char *target_path, int recurence){
	DIR *d = opendir(target_path);
	struct dirent *dir;
	if((check_directory(source_path) == 0 && check_directory(target_path) == 0) || recurence){

		printf("to jest recursive w synhro %d \n", recursive);
		while((dir = readdir(d))){
			if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0){
				continue;
			}
			char *joined_source_path = join_paths(source_path, dir->d_name);
			char *joined_target_path = join_paths(target_path, dir->d_name);
			if(check_directory(joined_target_path) == 0 && !recursive){
				printf("111\n");
				continue;
			}
			if(check_directory(joined_target_path) == 0){
				printf("222\n");
				delete(joined_source_path, joined_target_path, 1);
				printf("Jestem po delete\n");
				if(check_directory(joined_source_path) == -1){
					rmdir(joined_target_path);
					printf("555\n");
				}
			}
			else if(check_directory(joined_source_path) == -1){
				printf("333\n");
				remove(joined_target_path);
			}
		}
	}
	else{
		exit(EXIT_FAILURE);
	}

}

void on_signal(int sig)
{
    if (sig == SIGUSR1)
    {
        syslog(LOG_INFO, "Demon został obudzony za pomocą sygnału SIGUSR1");
    }
    if (sig == SIGTERM)
    {
        syslog(LOG_INFO, "Demon został zatrzymany");
        exit(EXIT_SUCCESS);
    }

    signal(sig, on_signal);
}


void demon(){
	pid_t pid;
	pid = fork();
	if (pid < 0) {
		exit(EXIT_FAILURE);
	}
	if (pid == 0)
	    {
	        signal(SIGUSR1, on_signal);
	        signal(SIGTERM, on_signal);
	    }
	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}


	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

}


int main (int argc, char *argv[]) {
	source_path = argv[1];
	target_path = argv[2];
	if(argc == 4){
		recursive = (strcmp(argv[3], "-R") == 0);
	}
	//demon();
	while(1){
		delete(source_path, target_path, 0);
		synchronize(source_path, target_path);
		sleep(60);
	}
	//size = get_size(source_path);
	//copy(source_path, target_path, size);
	return EXIT_SUCCESS;
}

