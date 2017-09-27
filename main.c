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

	struct addrinfo hints = { 0 }, *res, *p;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	char port_string[6];
	snprintf(port_string, 6, "%hu", GlobalConfiguration.port);
	if (getaddrinfo(NULL, port_string, &hints, &res) != 0) {
		debug_printf("Error getaddrinfo().\n");
		exit(1);
	}

	for (p = res; p != NULL; p = p->ai_next) {
		GlobalConfiguration.listenfd = socket(p->ai_family, p->ai_socktype, 0);
		if (GlobalConfiguration.listenfd == -1) continue;
		if (bind(GlobalConfiguration.listenfd, p->ai_addr, p->ai_addrlen) == 0) break;
	}

	if (p == NULL) {
		debug_printf("Failed to obtain socket.\n");
		exit(1);
	}

	freeaddrinfo(res);

	if (listen(GlobalConfiguration.listenfd, GlobalConfiguration.backlog)) {
		debug_printf("Failed to listen to socket.\n");
		exit(1);
	}

	debug_printf("Started server on port %hu with root directory %s.\n", GlobalConfiguration.port, GlobalConfiguration.root);

	// Infinite loop.
	for(;;) {
		struct sockaddr_in clientaddr;
		socklen_t addrlen = sizeof(clientaddr);
		int client;
		if (client = accept(GlobalConfiguration.listenfd, (struct sockaddr *)&clientaddr, &addrlen)) {
			if (client < 0) {
				debug_printf("Error aaccepting.\n");
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
