/**
 * FILENAME:	main.c
 * 
 * DESCRIPTION:
 * 		Contains the main function which will initialize and handle connection threads.
 * 		
 * Author:		Jake Thiem
 * Date:		September 27, 2017
 * 
 * Changes:
 * 
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "debug.h"
#include "parse_arguments.h"
 
int main(int argc, char **argv)
{
	// Parse arguments and initialize configurations.
	parse_arguments(argc, argv);

	if (GlobalConfiguration.daemonize) {
		pid_t pid = 0;
		pid_t sid = 0;

		pid = fork();
		if (pid < 0) {
			printf("Failed to fork.\n");
			exit(1);
		}

		if (pid > 0) {
			printf("PID of child process %d\n", pid);
			exit(0);
		}

		umask(0);
		sid = setsid();
		if (sid < 0) exit(1);

		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
	}

	debug_printf("Starting server on port %hu with root directory %s.\n", GlobalConfiguration.port, GlobalConfiguration.root);

	// Infinite loop.
	for(;;) {
		sleep(1);
		debug_printf("Logging.\n");
	}

	return 0;
}
