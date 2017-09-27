/**
 * FILENAME:	parse_arguments.c
 * 
 * DESCRIPTION:
 * 		Parses command line arguments and sets the configuration settings based on value.
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

#include "conf.h"

Config GlobalConfiguration = { 0 };

char *default_root = "/www";
char *default_index = "/index.html";

void parse_arguments(int argc, char **argv)
{
	char c;
	while ((c = getopt(argc, argv, "p:r:vdl:b:")) != -1) {
		switch (c) {
			case 'p':
				GlobalConfiguration.port = atoi(optarg);
				break;
			case 'r':
				GlobalConfiguration.root = malloc(strlen(optarg));
				strcpy(GlobalConfiguration.root,optarg);
				break;
			case 'v':
				GlobalConfiguration.verbose = true;
				break;
			case 'd':
				GlobalConfiguration.daemonize = true;
				break;
			case 'l':
				GlobalConfiguration.dolog = true;
				GlobalConfiguration.logfile = malloc(strlen(optarg));
				strcpy(GlobalConfiguration.logfile,optarg);
				break;
			case 'b':
				GlobalConfiguration.backlog = atoi(optarg);
				break;
			case '?':
				printf("Unknown argument %c.\n", c);
			default:
				exit(1);
		}
	}

	if (GlobalConfiguration.port == 0)
		GlobalConfiguration.port = 80;

	if (GlobalConfiguration.root == NULL) {
		GlobalConfiguration.root = default_root;
	}

	if (GlobalConfiguration.dolog) {
		GlobalConfiguration.logfp = fopen(GlobalConfiguration.logfile, "w+");
		if (GlobalConfiguration.logfp == NULL) {
			printf("Unable to open file %s.\n", GlobalConfiguration.logfile);
			exit(1);
		}
	}

	if (GlobalConfiguration.backlog == 0) {
		GlobalConfiguration.backlog = 1000000;
	}

	if (GlobalConfiguration.index == NULL) {
		GlobalConfiguration.index = default_index;
	}

	return;
}