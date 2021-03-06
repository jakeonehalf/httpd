/**
 * FILENAME:	send_response.c
 * 
 * DESCRIPTION:
 * 		Send response to client.
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
#include <string.h>
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

static void send_file(int client, char *path)
{
	int fd;
	if ((fd = open(path, O_RDONLY)) != -1) {
		send(client, "HTTP/1.0 200 OK\n\n", 17, 0);
		char data_to_send[1024];
		int bytes_read;
		while ((bytes_read = read(fd, data_to_send, 1024)) > 0) {
			debug_printf("%s\n", data_to_send);
			write(client, data_to_send, bytes_read);
		}
	} else {
		write(client, "HTTP/1.0 404 Not Found\n", 23);
	}
}

static void send_cgi(int client, char *path)
{
	FILE *fd;

	char command[1024] = { 0 };
	char data_to_send[1024];
	int bytes_read;

	debug_printf("Command: %s %s\n", GlobalConfiguration.cgi_bin, path);

	snprintf(command, 1024, "%s %s", GlobalConfiguration.cgi_bin, path);

	debug_printf("Command: %s\n", command);

	fd = popen(command, "r");

	if (fd == NULL) {
		debug_printf("Command unable to run %s.\n", command);
		return;
	}

	while (fgets(data_to_send, sizeof(data_to_send), fd)) {
		debug_printf("%s\n", data_to_send);
		write(client, data_to_send, strlen(data_to_send));
	}
}

void send_response(int client)
{
	char *message = malloc(sizeof(char) * GlobalConfiguration.backlog);
	int received = (int)recv(client, (void *)message, (size_t)GlobalConfiguration.backlog, (int)NULL);

	if (received < 0) {
		debug_printf("Error receiving.\n");
	} else if (received == 0) {
		debug_printf("Client disconnected unexpectedly.\n");
	} else {
		debug_printf("%s\n", message);

		char *request_line[3];

		request_line[0] = strtok(message, " \t\n");
		if (strncmp(request_line[0], "GET\0", 4) == 0) {
			request_line[1] = strtok(NULL, " \t");
			request_line[2] = strtok(NULL, " \t\n");
			if (strncmp(request_line[2], "HTTP/1.0", 8) != 0 && strncmp(request_line[2], "HTTP/1.1", 8) != 0) {
				write(client, "HTTP/1.0 400 Bad Request\n", 25);
			} else {
				if (strncmp(request_line[1], "/\0", 2) == 0)
					request_line[1] = GlobalConfiguration.index;

				char *path = malloc(sizeof(char) * (strlen(GlobalConfiguration.root) + strlen(request_line[1])));
				strcpy(path, GlobalConfiguration.root);
				strcpy(path + strlen(GlobalConfiguration.root), request_line[1]);

				debug_printf("Requested file: %s\n", path);

				if (strstr(path, ".cgi")) {
					send_cgi(client, path);
				} else {
					send_file(client, path);
				}

				free(path);
			}
		}
	}

	free(message);
	shutdown(client, SHUT_RDWR);
	close(client);
	return;
}