#include <stdio.h> // for printf and scanf
#include <stdlib.h>//for EXIT_SUCCESS
#include <unistd.h>
#include <sys/inotify.h>
#include <errno.h>
#include <stdbool.h>

int main(int args, char* argv[])
{
	bool opt_h = false;
	bool opt_d = false;
	bool opt_m = false;
	bool opt_t = false;
	if (args == 1)
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
		}
		else if (opt == 'm'){
			opt_m = true;
		}
		else if (opt == 't'){
			opt_t = true;
		}
		opt = getopt(argc, argv, "hd:mt"); //restated because to reiterated through while loop to make sure there are not more options
	}
	
	ssize_t EVENT_SIZE = (sizeof (struct inotify_event));
	ssize_t BUF_LEN = (1024 * (EVENT_SIZE + 16));
	int fd = inotify_init();
	char* path //the location of the file comes into path;
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