#include <stdio.h> // for printf and scanf
#include <stdlib.h>//for EXIT_SUCCESS
#include <unistd.h>
#include <sys/inotify.h>
#include <errno.h>
#include <stdbool.h>

int main(int args, char* argv[])
{
	bool opt_h = false;
	bool opt_m = false;
	bool option_t = false;
	bool opt_x = false;
	if (args == 1)
	{
		printf("Usage: This program will replicate and monitor the");
		printf(" file that is passed into the program as an argument\n");
		return EXIT_SUCCESS;
	}

	int opt = getopt(argc, argv, "hmtx");//for all options h,m,t and x
	while (opt != -1){
		if (opt == 'h'){
			opt_h = true;
		}
		else if (opt == 'm'){
			opt_m = true;
		}
		else if (opt == 't'){
			option_t = true;
		}
		else if (opt == 'x'){
			opt_x = true;
		}
		opt = getopt(argc, argv, "hmtx"); //restated because to reiterated through while loop to make sure there are not more options
	}

	ssize_t EVENT_SIZE = (sizeof (struct inotify_event));
	ssize_t BUF_LEN = (1024 * (EVENT_SIZE + 16));
	int fd = inotify_init();
	char* path = "/etc/passwd";
	int wd = inotify_add_watch(fd, path, IN_MODIFY | IN_ACCESS);
	int x;
	char buffer[BUF_LEN];


}