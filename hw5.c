#include <stdio.h> // for printf and scanf
#include <stdlib.h>//for EXIT_SUCCESS
#include <unistd.h>
#include <sys/inotify.h>
#include <errno.h>

int main(int args, char* argv[])
{
	if (args == 1)
	{
		printf("Usage: This program will replicate and monitor the");
		printf(" file that is passed into the program as an argument\n");
	}
}