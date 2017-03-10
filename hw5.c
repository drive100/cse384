#include <stdio.h> // for printf and scanf
#include <stdlib.h>//for EXIT_SUCCESS
#include <unistd.h>
#include <sys/inotify.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#include <utime.h>
#include <linux/limits.h>
#include <libgen.h>

void copy_file(const char* inpath,const char* outpath, bool n);

size_t modnum = 1;

int main(int argc, char* argv[])
{
	bool opt_h = false;
	bool opt_d = false;
	bool opt_m = false;
	bool opt_t = false;
	char* d_arg = NULL;
	bool backupchanged = false;
	char* backup_path;
	//size_t modnum = 1;//this number stores the number of times the user has modified the original file
	//used to name the backup files for revision

	if (argc == 1)
	{
		printf("Usage: This program will replicate and monitor the");
		printf(" file that is passed into the program as an argument\n");
		return EXIT_SUCCESS;
	}
	int opt = getopt(argc, argv, "hmtd:");//for all options h,d,m and t
	while (opt != -1){
		if (opt == 'h'){
			opt_h = true;
		}
		else if (opt == 'd'){
			opt_d = true;
			d_arg = optarg;
		}
		else if (opt == 'm'){
			opt_m = true;
		}
		else if (opt == 't'){
			opt_t = true;
		}
		opt = getopt(argc, argv, "hmtd:"); //restated because to reiterated through while loop to make sure there are not more options
	}
	if (opt_h == true){
		printf("You entered the help option!\n");
		printf("This program is for backing up files\n");
		printf("Entering -h will give you this help guide\n");
		printf("Entering -d will enable you to pick your backup location,\n");
		printf("Remember to enter your absolute path right after!\n");
		printf("Entering -m tells the program to only copy the contents of\n");
		printf("the file you want to be backed up and nothing else\n");
		printf("Entering -t will add the date/time to the duplication file's file name\n");
		printf("in other words, the copied file's name will be like \"file_20170305.... \"\n");
		return EXIT_SUCCESS;
	}

	if (opt_d == true){
		//d_arg = optarg;
		printf("optarg = %s\n", d_arg);
		if (d_arg == NULL){
			printf("You entered the -d option but did not enter a path.\n");
			printf("The default path will be used.\n");
		
}		else if (d_arg != NULL){
			//need to check whether or not the argument passed to -d is an actual path
			//if path is not a path, print out saying so and use default path
			//if path is an acutal path, update path variable
			

			if (access(d_arg, F_OK) == -1)
			{
				printf("error: '%s' does not exist\n", d_arg);
			}
			else
			{
				struct stat fd;
				if(stat(d_arg, &fd) == 0 && S_ISDIR(fd.st_mode))
				//if(stat(backup_path, &fd) == 0 && S_ISDIR(fd.st_mode))
				{
					backupchanged = true;
					backup_path = d_arg;
					printf("By default, the backup folder is changed to %s\n", backup_path);
				}
				else{
					printf("%s is not a folder\n", d_arg);
				//use default path;
				}
			}
		}
	}
	
	if (opt_t == true){
		//APPEND time to file name
		//using ISO 8601, no colons or timezone like in lab2
		//option should default to disabled
		//makes time structure
 	 time_t rawtime;
 	 struct tm * timeinfo;
 	 char buffer [80];

 	 time ( &rawtime );
 	 timeinfo = localtime( &rawtime );

 	 strftime (buffer,80," %Y%m%d%I%M%S.",timeinfo);

 	 puts (buffer);

	printf("Created file with appended time");

	}

	ssize_t EVENT_SIZE = (sizeof (struct inotify_event));
	ssize_t BUF_LEN = (1024 * (EVENT_SIZE + 16));
	int fd = inotify_init();
	if (backupchanged == false)
	{
		backup_path = "/home/yihong/Desktop";
		printf("The default backup folder is %s\n", backup_path);
	}
	const char* path = argv[optind];//the location of the input file;
	//location of the backup file is in backup_path
	if (access(path, F_OK) == -1)
	{
		printf("error: '%s' does not exist\n", path);
		return EXIT_SUCCESS;
	}
	if (access(path, R_OK) == -1)
	{
		printf("error: '%s' is unreadable\n", path);
		return EXIT_SUCCESS;
	}
	// if (access(backup_path, F_OK) == -1)
	// {
	// 	printf("error: '%s' does not exist\n", backup_path);
	// 	return EXIT_SUCCESS;
	// }
	printf("path = %s\nbackup_path = %s\n", path, backup_path);
	copy_file(path, backup_path, opt_m);
	int wd = inotify_add_watch(fd, path, IN_MODIFY | IN_DELETE);
	int x;
	char buffer[BUF_LEN];

	while (1){
		x = read(fd, buffer, BUF_LEN);
		if (x < 0 || wd < 0 || fd < 0)
		{
			perror("inotify error");
			return EXIT_FAILURE;
		}
		for (char* p = buffer; p < buffer + x;)
		{
			struct inotify_event* event = (struct inotify_event*)p;
			if (event->mask & IN_MODIFY)
			{
				printf("The file %s is modified\n", path);
			}
			if (event->mask & IN_DELETE)
			{
				printf("The file %s is deleted\n", path);
				return EXIT_SUCCESS;
			}

			p += sizeof(struct inotify_event) + event->len;
		}
	}

}

void copy_file(const char* inpath,const char* outpath, bool n)
{
	//outpath is directory of output (copy file)
	const char* filename = basename(inpath);
	const size_t data_size = 120;
	char data[data_size];
	int outft, inft, fileread = 1;

	size_t rev = modnum;
	char* append =  "backup_rev%d";
	char backup_buff[10];
	snprintf(rev_buff, 10, "backup_rev")

	char rev_buff[10];
	snprintf(rev_buff, 10, "_rev%d", rev);
	
	char buffer[PATH_MAX+10];
	strcpy(buffer, outpath);
	strcat(buffer, rev_buff);
	printf("%s\n", buffer);



	//create a output file, and the file will be in outpath
	outft = (open(outpath, O_CREAT | O_APPEND | O_RDWR));
	if(outft == -1)
	{
		perror("outpath open");
		exit(EXIT_FAILURE);
	}
	//open the input file, and the file will be in inpath
	inft = (open(inpath, O_RDONLY, S_IRWXG | S_IRWXU | S_IRWXO));
	if(inft == -1)
	{
		perror("inpath open");
		exit(EXIT_FAILURE);
	}
	while(fileread != 0){
		fileread = read(inft, data, data_size);
		if(fileread == -1);
		{
			perror("read");
			exit(EXIT_FAILURE);
		}
		if (write(outft, data, fileread) == -1)
		{
			perror("write");
			exit(EXIT_FAILURE);
		}
		printf("fileread = %d\n", fileread);
		
	}
	close(inft);
	close(outft);

	if(n == false)
	{
		struct stat *buf;
		buf = malloc(sizeof(struct stat));
		int stat_i;
		stat_i = stat(inpath, buf);
		if (stat_i == -1)
		{
			perror("stat");
			exit (EXIT_FAILURE);
		}
	//change permission bits
		if (chmod(outpath, buf->st_mode) == -1)
		{
			perror("chmod");
			exit(EXIT_FAILURE);
		}
	//change ownership
		if (chown(outpath, buf->st_uid, buf->st_gid))
		{
			perror("chown");
			exit(EXIT_FAILURE);
		}
		struct utimbuf new_times;
		new_times.modtime = buf->st_mtime;
		new_times.actime = buf->st_ctime;
		if (utime(outpath, &new_times) == -1)
		{
			perror("utime");
			exit(EXIT_FAILURE);
		}
	}
}
