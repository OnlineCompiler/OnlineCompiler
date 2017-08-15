/*************************************************************************
	> File Name: httpd.h
	> Author: XH
	> Mail: X_H_fight@163.com 
	> Created Time: Sun 02 Apr 2017 02:03:10 PM CST
 ************************************************************************/

#ifndef __HTTPD_H__
#define __HTTPD_H__

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <syslog.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define _SIZE_ (1024*4)
#define _STATUS_SIZE_ 20
//日志等级
typedef enum logGrade
{
	NORMAL,
	WARNING,
	FATAL,
}logGrade;


int StartUp(const char *ip, int port);
void PrintLog(const char *logMsg, logGrade grade);
int Handle_Request(int sock);
void EchoErrno(int sock, int statusCode);
int ExcuCgi(int sock, const char *method, \
		const char *path, const char *resource);

int ShowPage(int sock, const char *path, ssize_t size);

#endif
