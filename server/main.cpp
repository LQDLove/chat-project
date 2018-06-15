#include<iostream>
#include<errno.h>
#include<stdlib.h>
#include "tcpsever.h"
#include<string.h>
#include<signal.h>
using namespace std;

void fun(int sig)
{
	cout<<"客户端意外终止!"<<endl;
}

int main(int argc,char **argv)
{
	if(argc < 4)
	{
		cerr<<"arg not enough;errno:"<<errno<<endl;
		return 0;
	}
	//解析参数   ip   port   pth_num
	char ip[MAX] = {0};
	strcpy(ip,argv[1]);
	unsigned short port =(unsigned short) atoi(argv[2]);
	int pth_num = atoi(argv[3]);
	
	//构造服务器对象
	Tcpsever ser(ip,port,pth_num);
	signal(SIGPIPE,SIG_IGN);
	//运行服务器
	ser.run();
	return 0;
}
