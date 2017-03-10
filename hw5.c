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
#include <pwd.h>
#include <string.h>

void copy_file(const char* inpath,char* outpath, bool n, bool t);

static size_t modnum = 0;

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
		//prints helpful information
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
			//need to check whether or not the argument passed to -d is an actual path
			//if path is not a path, print out saying so and use default path
			//if path is an actual path, update path variable
		if (d_arg == NULL){
			printf("You entered the -d option but did not enter a path.\n");
			printf("The default path will be used.\n");
		}		
		else if (d_arg != NULL){
			if (access(d_arg, F_OK) == -1)
			{
				printf("error: '%s' does not exist\n", d_arg);
			}
			else
			{
				struct stat fd;
				if(stat(d_arg, &fd) == 0 && S_ISDIR(fd.st_mode))
				{
					backupchanged = true;
					backup_path = d_arg;
					printf("By default, the backup folder is changed to %s\n", backup_path);
				}
				else{
					printf("%s is not a folder\n", d_arg);
				}
			}
		}
	}
	if (opt_t == true){
	printf("Cpying the original file with appended time\n");

	}

	ssize_t EVENT_SIZE = (sizeof (struct inotify_event));
	ssize_t BUF_LEN = (1024 * (EVENT_SIZE + 16));
	int fd = inotify_init();
	if (backupchanged == false)
	{
		struct passwd *pw = getpwuid(getuid());
		backup_path = pw->pw_dir;
		strcat(backup_path, "/Desktop");
		printf("The default backup folder is %s\n", backup_path);
	}
	const char* path = argv[optind]; //location of the input file;
	if (path == NULL)
	{
		printf("You did not give in a file\n");
		return EXIT_SUCCESS;
	}
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
	copy_file(path, backup_path, opt_m, opt_t);
	int wd = inotify_add_watch(fd, path, IN_MODIFY | IN_DELETE_SELF);
	int x;
	char buffer[BUF_LEN];
	int mod = 0;

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
				mod++;
				printf("%d = The file %s is modified\n",mod, path);
				copy_file(path, backup_path, opt_m, opt_t);
			}
			if (event->mask & IN_DELETE_SELF)
			{
				printf("The file %s is deleted\n", path);
				return EXIT_SUCCESS;
			}
			p += sizeof(struct inotify_event) + event->len;
		}
	}
}

void copy_file(const char* inpath,char* outpath, bool n, bool t)
{
	//outpath is directory of output (copy file)
	//printf("inpath = ~~~~~~~~~~~   %s\n", inpath);
	char* file;
	file = strdup(inpath);
	char* filename = basename(file);
	printf("filename = %s, file = %s, inpath = %s\n", filename, file, inpath);
	const size_t data_size = 120;
	char data[data_size];
	int outft, inft, fileread = 1;
	size_t rev = modnum;
	char rev_buff[100];
	time_t rawtime;
	//char* append =  "_rev%d";
	if (t == false)
	{
		char backup_buff[10];
	//snprintf(rev_buff, 10, "backup_rev")
		snprintf(rev_buff, 10, "_rev%d", rev);
		char buffer[PATH_MAX+10];
		strcpy(buffer, filename);
		strcat(buffer, rev_buff);
		printf("%s\n", buffer);
		filename = buffer;
		printf("filename = buffer = %s\n", filename);
	}
	else
	{
		printf("debugging ~~~~~~~~~~~~~~~dsdf~~2\n");
		struct tm *timeinfo;
		printf("debugging ~~~~~~~~~~sdf~~~~~~~2\n");
		time(&rawtime);
		printf("debugging ~~~sdf~~~~~~~~~~~~~~2\n");
		timeinfo = localtime(&rawtime);
		//time ( &rawti
		printf("debugging ~~~~~~~~~~~~~~~~~2\n");
		sprintf(rev_buff,"_%d%d%d%d%d%d",timeinfo->tm_year,timeinfo->tm_mon,
			timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, 
			timeinfo->tm_sec);
		printf("debugging ~~~~~~~~~~~~~~~~~3\n");
		//strftime (rev_buff,100,"%Y%m%d%I%M%S.",timeinfo);
		puts (rev_buff);
		strcat(filename, rev_buff);
		printf("debugging ~~~~~~~~~~~~~~~~~4\n");
	}
	printf("filename = %s\n", filename);
	//free(buffer);
	//snprintf(
	snprintf(rev_buff, 100, "/%s", filename);
	char buffer1[PATH_MAX+10];
	strcpy(buffer1, outpath);
	strcat(buffer1, rev_buff);
	outpath = buffer1;
	printf("outpath = %s\n", outpath);
	modnum++;
	printf("debugging ~~~~~~~~~~~~~~~~~5\n");
	//free(filename);
	//create a output file, and the file will be in outpath
	outft = (open(outpath, O_CREAT | O_APPEND | O_RDWR,S_IRWXG | S_IRWXU | S_IRWXO));
	if(outft == -1)
	{
		perror("outpath open");
		exit(EXIT_FAILURE);
	}
	//open the input file, and the file will be in inpath
	inft = (open(inpath, O_RDONLY));
	if(inft == -1)
	{
		printf("%s\n", inpath);
		perror("inpath open");
		exit(EXIT_FAILURE);
	}
	while(fileread != 0){
		fileread = read(inft, data, data_size);
		if(fileread == -1)
		{
			perror("read");
			exit(EXIT_FAILURE);
		}
		if (write(outft, data, fileread) == -1)
		{
			perror("write");
			exit(EXIT_FAILURE);
		}
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

		//source: stackoverflow.com/questions/2185338/how-to-set-the-modification-time-of-a-file-programmatically
		struct utimbuf new_times;
		new_times.modtime = buf->st_mtime;
		new_times.actime = buf->st_ctime;
		if (utime(outpath, &new_times) == -1)
		{
			perror("utime");
			exit(EXIT_FAILURE);
		}
		free(buf);
	}
}
