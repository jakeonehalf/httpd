/**
 * FILENAME:	conf.h
 * 
 * DESCRIPTION:
 * 		Contains the structure for configuration values.
 * 		
 * Author:		Jake Thiem
 * Date:		September 27, 2017
 * 
 * Changes:
 * 
 */
 
typedef struct Config_t {
	uint16_t 	port;
	char 		*root;
	bool		verbose;
	bool		daemonize;
	bool		dolog;
	char		*logfile;
	FILE		*logfp;
} Config;