#include <stdio.h> // for printf and scanf
#include <stdlib.h>//for EXIT_SUCCESS
#include <unistd.h>
#include <sys/inotify.h>
#include <errno.h>
#include <stdbool.h>

int main(int argc, char* argv[])
{
	bool opt_h = false;
	bool opt_d = false;
	bool opt_m = false;
	bool opt_t = false;
	char* d_arg = NULL;
	if (argc == 1)
	{
		printf("Usage: This program will replicate and monitor the");
		printf(" file that is passed into the program as an argument\n");
		return EXIT_SUCCESS;
	}
	int opt = getopt(argc, argv, "hd:mt");//for all options h,d,m and t
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
		opt = getopt(argc, argv, "hd:mt"); //restated because to reiterated through while loop to make sure there are not more options
	}
	if (opt_h = true){
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

	if (opt_d = true){

		if (d_arg == NULL){
			printf("You entered the -d option but did not enter a path.\n");
			printf("The default path will be used.");
		}
		else if (d_arg == NULL){
			//need to check whether or not the argument passed to -d is an actual path
		}
	}
	ssize_t EVENT_SIZE = (sizeof (struct inotify_event));
	ssize_t BUF_LEN = (1024 * (EVENT_SIZE + 16));
	int fd = inotify_init();
	const char* path = argv[optind];//the location of the file comes into path;
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
			//printf("printing something \n");
			struct inotify_event* event = (struct inotify_event*)p;
			//if (event -> len)
			//{
				if (event->mask & IN_MODIFY)
				{
					printf("The file %s is modified\n", path);
				}
				if (event->mask & IN_DELETE)
				{
					printf("The file %s is deleted\n", path);
					return EXIT_SUCCESS;
				}
			//}

			p += sizeof(struct inotify_event) + event->len;
		}
	}

}