#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "cJSON.h"

char* _itoa(int num,char* str,int radix)
{/*索引表*/
	char index[]="0123456789ABCDEF";
	unsigned unum;/*中间变量*/
	int i=0,j,k;
	/*确定unum的值*/
	if(radix==10 && num < 0)/*十进制负数*/
	{
		unum = (unsigned) - num;
		str[i++] = '-';
	}
	else unum = (unsigned)num;/*其他情况*/
	/*转换*/
	do{
		str[i++] = index[unum%(unsigned)radix];
		unum /= radix;
	}while(unum);
	str[i] = '\0';
	/*逆序*/
	if(str[0]=='-')k=1;/*十进制负数*/
	else k=0;
	char temp;
	for(j=k;j<=(i-1)/2;j++)
	{
		temp = str[j];
		str[j] = str[i-1+k-j];
		str[i-1+k-j] = temp;
	}
	return str;
}

void compiler(const char* jsonStr)
{
	time_t now;
	char t[32], p[8];
	_itoa(time(&now), t, 16);
	_itoa(getpid(), p, 16);
	char exename[64], filename[64], asmname[64];

	sprintf(exename, "tmp/%s%s", t, p);
	sprintf(filename, "tmp/%s%s.cpp", t, p);
	sprintf(asmname, "tmp/%s%s.s", t, p);

	//printf("%s %s %s\n", exename, filename, asmname);
	int fd = open(filename, O_CREAT|O_RDWR, S_IWUSR|S_IRUSR);
	char s[12], W[7], S[4], O[5]; s[0] = 0; W[0] = 0; S[0] = 0; O[0] = 0;

	cJSON* json = cJSON_Parse(jsonStr);
	if(!json)//error
	{
		printf("jsonStr error\n"); //print_log
		return;
	}
	else
	{
		//source
		cJSON* src = cJSON_GetObjectItem(json,"source"); 
		if(src)
		{
			//printf("%s\n",src->valuestring);
			write(fd, src->valuestring, strlen(src->valuestring));
		}
		//standard   -std=c++11
		cJSON* std = cJSON_GetObjectItem(json,"standard"); 
		if(std)
		{
			if(strcasecmp(std->valuestring, "c11")==0)
				strcpy(s, "-std=c++11 ");
			else if(strcasecmp(std->valuestring, "c98")==0)
				strcpy(s, "-std=c++98 ");
		}

		//Wall
		if(cJSON_GetObjectItem(json,"Wall"))
			strcpy(W, "-Wall ");
		//S
		if(cJSON_GetObjectItem(json,"S"))
			strcpy(S, "-S ");
		//O2
		if(cJSON_GetObjectItem(json,"O2"))
			strcpy(O, "-O2 ");
	}

	//-std=c++11 -Wall -S -O2
	//printf("%s %s %s %s\n", s, W, S, O);
	cJSON_Delete(json); 

	char cmd[256];
	strcpy(cmd, "g++ ");
	if(strcmp(S, "") != 0)
	   strcat(cmd, S);
	if(strcmp(s, "") != 0)
	   strcat(cmd, s);
	if(strcmp(W, "") != 0)
	   strcat(cmd, W);
	if(strcmp(O, "") != 0)
	   strcat(cmd, O);
	strcat(cmd, filename);
	strcat(cmd, " -o ");
	if(strcmp(S, "") != 0)
	   strcat(cmd, asmname);
	else
	   strcat(cmd, exename);

	//printf("%s\n", cmd);
	int ret = system(cmd);
	if(!ret)
	{
		if(strcmp(S, "") != 0)
		{
			char cmds[256];
			strcpy(cmds, "cat ");
			strcat(cmds, asmname);
			system(cmds);
		}
		else
		{
			system(exename);
		}
	}

}

int main()
{
	int content_len = atoi(getenv("CONTENTLENGTH"));
	//printf("%d\n", content_len);
	char* query = (char*)malloc(content_len+1);
	ssize_t s = read(0, query, content_len);
	if(s > 0)
		query[s] = 0;
	//printf("query: %s  ", query);
	compiler(query);
	free(query);
	return 0;
}

