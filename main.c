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
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>

#include "debug.h"
#include "parse_arguments.h"
#include "send_response.h"

static volatile bool running = true;

void exit_handler(int v)
{
	running = false;
	debug_printf("Received close signal.\n");

	close(GlobalConfiguration.listenfd);
	exit(0);
}
 
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

	struct sockaddr_in server_address = { 0 };

	GlobalConfiguration.listenfd = socket(AF_INET, SOCK_STREAM, 0);

	if (GlobalConfiguration.listenfd < 0) {
		debug_printf("Unable to open socket.\n");
		exit(1);
	}

	debug_printf("Listen FD = %d\n", GlobalConfiguration.listenfd);

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(GlobalConfiguration.port);

	if (bind(GlobalConfiguration.listenfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
		debug_printf("Unable to bind to socket.\n");
		exit(1);
	}

	if (listen(GlobalConfiguration.listenfd, GlobalConfiguration.backlog)) {
		debug_printf("Failed to listen to socket.\n");
		exit(1);
	}

	debug_printf("Started server on port %hu with root directory %s.\n", GlobalConfiguration.port, GlobalConfiguration.root);

	signal(SIGINT, exit_handler);

	// Infinite loop.
	while (running) {
		struct sockaddr_in clientaddr;
		socklen_t addrlen = sizeof(clientaddr);
		int client;
		if (client = accept(GlobalConfiguration.listenfd, (struct sockaddr *)&clientaddr, &addrlen)) {
			if (client < 0) {
				debug_printf("Error accepting.\n");
				continue;
			}

			if (fork() == 0) {
				send_response(client);
				exit(0);
			}
		}
	}

	return 0;
}
