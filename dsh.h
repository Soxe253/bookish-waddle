#define MAXBUF 256  // max number of characteres allowed on command line

void dsh();

char** split(char *str, char *delim, int numTokens, int length);

void freearray(char** array, int length);