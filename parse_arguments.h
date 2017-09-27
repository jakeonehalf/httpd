/**
 * FILENAME:	parse_arguments.h
 * 
 * DESCRIPTION:
 * 		Header file for parse_arguments.c which contains function prototypes
 * 		and a pointer to the global configuration structure.
 * 		
 * Author:		Jake Thiem
 * Date:		September 27, 2017
 * 
 * Changes:
 * 
 */

#ifndef PARSE_ARGUMENTS_H
#define PARSE_ARGUMENTS_H

#include "conf.h"

extern Config GlobalConfiguration;

void parse_arguments(int argc, char **argv);

#endif
