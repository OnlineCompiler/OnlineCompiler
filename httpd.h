#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define SIZE 8192

enum LEVEL
{
	SUCCESS,
	NOTICE,
	WARNING,
	ERROR,
	FATAL,
};

int startup(int port);
void print_log(char* error, int level, char* file, int line);
int request_handle(int sock);
