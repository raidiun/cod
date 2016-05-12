// DEBUG definitions

#include <stdlib.h>
#include <stdio.h>

#include <errno.h>

#define ERROREXIT(MSG,NUM) fprintf(stderr,MSG " : %i\r\n", errno); exit(NUM)

#ifdef DEBUG
	#define DEBUGMSG(...) fprintf(stderr,__VA_ARGS__)
#else
	#define DEBUGMSG(...)
#endif