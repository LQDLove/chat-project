#include<iostream>
#include<errno.h>
#include<stdlib.h>
#include "tcpsever.h"
#include<string.h>
#include<signal.h>
using namespace std;

void fun(int sig)
{
	cout<<"客户端意外终止"<<endl;
	signal(sig,SIG_IGN);
}

int main(int argc,char **argv)
{
	//解析参数   ip   port   pth_num
	char ip[MAX] = "127.0.0.1";
//	strcpy(ip,argv[1]);
//	unsigned short port =(unsigned short) atoi(argv[2]);
//	int pth_num = atoi(argv[3]);
	
	//构造服务器对象
	Tcpsever ser(ip,6000,3);
	signal(SIGPIPE,fun);
	//运行服务器
	ser.run();
	return 0;
}
