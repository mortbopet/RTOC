#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif


// Global abort flag
static volatile int _quit = 0;

// Handler for signals liek <CRTL>+C
void signalHandler( int signal )
{
	switch ( signal ) {
#ifndef _WIN32
		case SIGHUP:
#endif
		case SIGINT:
		case SIGTERM:
			printf( "program stopped by user %d.\n", signal );
			if ( _quit )
				abort();
			_quit = 1;
			break;

		default:
			printf( "unhandled signal %d.\n", signal );
	}
} 

// Register ^C, HUP and TERM handler
void registerSignalHandler() 
{
#ifndef _WIN32
	signal( SIGHUP, signalHandler );
#endif
	signal( SIGINT, signalHandler );
	signal( SIGTERM, signalHandler ); 
}

void unregisterSignalHandler()
{
	signal( SIGTERM, SIG_DFL );
	signal( SIGINT, SIG_DFL );
#ifndef _WIN32
	signal( SIGHUP, SIG_DFL );
#endif 
}

