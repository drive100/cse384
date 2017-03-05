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
	bool opt_t = false;
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
		else if (opt == 'u'){
			opt_t = true;
		}
		else if (opt == 'x'){
			opt_x = true;
		}
		opt = getopt(argc, argv, "hmtx"); //restated because to reiterated through while loop to make sure there are not more options
	}


	
}