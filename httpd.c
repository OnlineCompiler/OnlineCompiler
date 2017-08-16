#include "httpd.h"
#include <sys/sendfile.h>
#include <ctype.h>
#include <sys/stat.h>

//打印日志
void print_log(char* error, int level, char* file, int line)
{
	time_t now; //当前时间戳
	time(&now);
	struct tm* timenow = localtime(&now);

	//openprint_log sysprint_log closeprint_log
	const char* log_level[5] = {"SUCCESS", "NOTICE", "WARNING", "ERROR", "FATAL"};
	int fd = open("log/httpd.log", O_CREAT|O_WRONLY|O_APPEND, S_IWUSR|S_IRUSR);
	char buf[1024];
	sprintf(buf, "[%s] %s\t\t%s(%d): %s\n", log_level[level], asctime(timenow), file, line, error);
	write(fd, buf, strlen(buf));
	close(fd);
}

//返回状态码
static void status_code(int sock, int status)
{
	const char* reason;//状态码描述
	switch(status)
	{
		case 400:
			reason = "Bad Request";
			break;
		case 403:
			reason = "Forbidden";
			break;
		case 404:
			reason = "Not Found";
			break;
		case 500:
			reason = "Internal Server Error";
			break;
		case 503:
			reason = "Server Unavailable";
			break;
		default:
			print_log("status code error", WARNING, __FILE__, __LINE__);
			reason = "Internal Server Error";
			break;
	}

	char buf[SIZE/8];
	sprintf(buf, "HTTP/1.0 %d %s\r\n", status, reason);
	send(sock, buf, strlen(buf), 0);
	const char* head = "Content-Type: text/html\r\n";
	send(sock, head, strlen(head), 0);
	send(sock, "\r\n", 2, 0);
	sprintf(buf, "<html><head><title>%d %s</title></head><body><h2><center>%d %s</center></h2><hr/></body></html>", 
				status, reason, status, reason);
	send(sock, buf, strlen(buf), 0);
}

static ssize_t read_line(int sock, char* buf, size_t size);
//清除消息报头
static void clear_header(int sock)
{
	char buf[SIZE];
	ssize_t size = 0;

	do{
		size = read_line(sock, buf, SIZE);
	}while(size != 1 || strcmp(buf, "\n") != 0);
}

//启动服务
int startup(int port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		print_log(strerror(errno), FATAL, __FILE__, __LINE__);
		exit(1);
	}

	int opt = 1;
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	  print_log(strerror(errno), WARNING, __FILE__, __LINE__);

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = inet_addr("0");
	local.sin_port = htons(port);

	if(bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0)
	{
		print_log(strerror(errno), FATAL, __FILE__, __LINE__);
		exit(2);
	}

	if(listen(sock, 10) < 0)
	{
		print_log(strerror(errno), FATAL, __FILE__, __LINE__);
		exit(3);
	}

	print_log("listen success", SUCCESS, __FILE__, __LINE__);
	return sock;
}

//读取请求行报头行
static ssize_t read_line(int sock, char* buf, size_t size)
{
	ssize_t i = 0; //ret
	ssize_t s = 0; //recv ret
	char c = 0;

	while(i < size - 1 && c != '\n')
	{
		s = recv(sock, &c, 1, 0);
		//  \r和\r\n -> \n
		if(s > 0 && c == '\r')
		{
			s = recv(sock, &c, 1, MSG_PEEK);
			if(s > 0 && c == '\n')
			  recv(sock, &c, 1, 0);
			else
			  c = '\n';
		}
		buf[i++] = c;
	}

	buf[i] = '\0';
	return i;
}

//处理cgi请求
static int exec_cgi(int sock, const char* method, const char* path, const char* query)
{
	char method_env[SIZE/16];
	char query_env[SIZE/2];
	char content_len_env[SIZE/8];
	//处理http请求头
	int content_len = 0;//请求正文长度
	char buf[SIZE/8];
	if(strcasecmp(method, "GET")  == 0)
	  clear_header(sock);
	else //POST
		while(read_line(sock, buf, sizeof(buf)) != 1)
			if(strncasecmp(buf, "Content-Length: ", 16) == 0)
			{
				content_len = atoi(buf+16);
				clear_header(sock);
				break;
			}

	//发送应答头部
	const char* respond_line = "HTTP/1.0 200 OK\r\n";
	send(sock, respond_line, strlen(respond_line), 0);
	const char* content_type = "Content-Type: text/html;charset=UTF-8\r\n";
	send(sock, content_type, strlen(content_type), 0);
	send(sock, "\r\n", 2, 0);

	//socketpair进行双向通信
	int sv[2];
	if(socketpair(PF_LOCAL, SOCK_STREAM, 0, sv) < 0)
	{
		status_code(sock, 500);
		print_log("socketpair", FATAL, __FILE__, __LINE__);
		return 4;
	}

	//fork子进程execl
	pid_t pid = fork();
	if(pid < 0)
	{
		status_code(sock, 500);
		print_log("fork", FATAL, __FILE__, __LINE__);
		return 5;
	}
	else if(pid == 0) //child
	{
		//关闭不用的fd
		close(sock);
		close(sv[0]);
		//文件描述符重定向
		dup2(sv[1], 1);
		dup2(sv[1], 0);
		//环境变量传递参数
		//1.方便解析字符串2.传送数据量少3.exec后的进程仍然可以看到
		sprintf(method_env, "METHOD=%s", method);
		putenv(method_env);

		if(strcasecmp(method, "GET") == 0)
		{
			sprintf(query_env, "QUERY=%s", query);
			putenv(query_env);
		}
		else //POST
		{
			sprintf(content_len_env, "CONTENTLENGTH=%d", content_len);
			putenv(content_len_env);
		}

		//程序替换
		execl(path, path, NULL);
		print_log("execl is error", FATAL, __FILE__, __LINE__);
		exit(6);
	}
	else //father
	{
		close(sv[1]);
		//读取POST方法正文内容通过管道发送给子进程
		char buf[content_len+1];
		if(strcasecmp(method, "POST") == 0)
		{
			//从消息正文读 可能buf存不下
			if(recv(sock, buf, content_len, 0) < 0)
			  print_log("recv failed", FATAL, __FILE__, __LINE__);

			//写到管道
			if(write(sv[0], buf, content_len) < 0)
			  print_log("write failed", FATAL, __FILE__, __LINE__);
		}

		//从管道读取内容发给sock
		ssize_t s;
		while((s = read(sv[0], buf, sizeof(buf))) > 0)
		{
			if(send(sock, buf, s, 0) < 0)
			  print_log("send failed", FATAL, __FILE__, __LINE__);
		}

		waitpid(-1, NULL, 0);
	}
	return 0;
}

//发送普通文件
static int send_file(int sock, const char* path, ssize_t size)
{	
	int fd = open(path, O_RDONLY);
	if(fd < 0)
	{
		print_log("open for read file failed", ERROR, __FILE__, __LINE__);
		status_code(sock, 404);
		close(fd);
		return -1;
	}

	send(sock, "HTTP/1.0 200 OK\r\n", 17, 0);
	send(sock, "\r\n", 2, 0);

	if(sendfile(sock, fd, NULL, size) < 0)
	{
		print_log("sendfile failed", ERROR, __FILE__, __LINE__);
		status_code(sock, 404);
		close(fd);
		return -2;
	}

	close(fd);
	return 0;
}


int request_handle(int sock)
{	
	char line[SIZE]; //存储一行内容
	char* c = line;
	char method[8]; //存储请求方法 
	char url[SIZE];
	char path[SIZE]; //请求路径
	char* query_str = NULL; //指向GET方法的参数
	int cgi = 0; //是否为cgi模式
	int ret = 0;

	ssize_t size = read_line(sock, line, sizeof(line));//读一行
	if(size == 0)
	{
		clear_header(sock); //读取失败清理掉header
		status_code(sock, 400);
		print_log("request error", ERROR, __FILE__, __LINE__);
		ret = 1;
	}

	//读取到第一行，获取请求方法
	int i = 0;
	while(i < 8 && !isblank(*c))
		method[i++] = *c++;
	method[i] = '\0';

	//获取url
	c++;
	i = 0;
	while(i < SIZE && !isblank(*c))
	  url[i++] = *c++;
	url[i] = '\0';

	//设置cgi模式
	//1. POST方法肯定是cgi模式
	//2. GET方法携带参数是cgi模式
	
	if(strcasecmp("POST", method) == 0)
	  cgi = 1;
	else if(strcasecmp("GET", method) == 0)
	{
		query_str = url;
		while(*query_str && *query_str != '?')
		  ++query_str;
		if(*query_str == '?')
		{
			*query_str = '\0';
			++query_str;
			cgi = 1;
		}
	}
	else //既不是GET也不是POST
	{
		clear_header(sock);
		status_code(sock, 400);
		print_log("请求方法未知", WARNING, __FILE__, __LINE__);
		ret = 2;
	}

	//处理请求路径
	sprintf(path, "wwwroot%s", url);
	if(path[strlen(path)-1] == '/') //请求path以/结尾是目录
	  strcat(path, "index.html");
	
	struct stat st;
	//int stat(const char *path, struct stat *buf);
	if(stat(path, &st) < 0) //获取文件信息保存在stat结构体中
	{
		clear_header(sock);
		status_code(sock, 404);
		ret = 3;
		goto END;
	}

	//获取path文件信息成功
	if(S_ISDIR(st.st_mode)) //文件是否是目录
	  strcat(path, "/index.html");
	//文件是否为可执行文件
	else if(st.st_mode & S_IXUSR || st.st_mode & S_IXGRP || st.st_mode & S_IXOTH)
	  cgi = 1;

	//处理cgi和非cgi模式
	if(cgi)
		ret = exec_cgi(sock, method, path, query_str);
	else
	{
		clear_header(sock);
		ret = send_file(sock, path, st.st_size);
	}

END:
	close(sock);
	return ret;
}
