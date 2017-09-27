/**
 * FILENAME:	debug.h
 * 
 * DESCRIPTION:
 * 		Contains functionality to provide verbose printing and logging.
 * 		
 * Author:		Jake Thiem
 * Date:		September 27, 2017
 * 
 * Changes:
 * 
 */
 
#ifndef _DEBUG_H_
#define _DEBUG_H_

#define debug_printf(format,...) \
		do { 																	\
			if (GlobalConfiguration.verbose && !GlobalConfiguration.daemonize) {\
				printf(format, ##__VA_ARGS__);									\
			}																	\
			if (GlobalConfiguration.dolog) {									\
				fprintf(GlobalConfiguration.logfp, format, ##__VA_ARGS__);		\
				fflush(GlobalConfiguration.logfp);								\
			}																	\
		} while(0)																\

#endif
