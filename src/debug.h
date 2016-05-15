// DEBUG definitions

#include <stdlib.h>
#include <stdio.h>

#include <errno.h>

#define ERROREXIT(MSG,NUM) fprintf(stderr,MSG " : %i\r\n", errno); exit(NUM)

#ifndef QUIETERROR 
  #define ERRORMSG(...) fprintf(stderr,__VA_ARGS__) 
#else 
  #define ERRORMSG(...) 
#endif 

#ifdef DEBUG
	#define DEBUGMSG(...) fprintf(stderr,__VA_ARGS__)
#else
	#define DEBUGMSG(...)
#endif

#define DHTD_ERROR -1