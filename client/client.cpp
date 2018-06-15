#include"client.h"
#include"public.h"
#include"view_cli.h"
#include<iostream>
#include<string>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<json/json.h>
#include<errno.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<signal.h>
using namespace std;
//_sockfd  msg
Client::Client(char*ip,unsigned short port)
{
	_sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(_sockfd == -1)
	{
		cerr<<"socket err"<<endl;
		return;
	}

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip);
	int res = connect(_sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
	if(res == -1)
	{
		cerr<<"connect fail:errno"<<errno<<endl;
		return;
	}
	
}

void Client::start_login_register(int flag)
{
	char name[32]={0};
	cout<<"请输入用户名:"<<endl;
	while(1)
	{
		cin>>name;
		fflush(stdin);
		if(strlen(name) > 20)
		{
			cout<<"用户名太长，请重新输入!"<<endl;
			continue;
		}
		break;
	}
	cout<<"请输入密码:"<<endl;
	char pw[32] = {0};
	while(1)
	{
		cin>>pw;
		fflush(stdin);
		if(strlen(pw) > 20)
		{
			cout<<"密码太长,请重新输入(8~12位)!"<<endl;
			continue;
		}
		break;
	}
	if(flag == 0)//register
	{
		Json::Value val;
		val["TYPE"] = MSG_TYPE_REGISTER;
		val["name"] = name;
		val["pw"] = pw;
		if(-1 == send(_sockfd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
		{
			cerr<<"send fail;errno:"<<errno<<endl;
			return;
		}
	}
	if(flag == 1)//login
	{
		Json::Value val;
		val["TYPE"] = MSG_TYPE_LOGIN;
		val["name"] = name;
		val["pw"] = pw;
	
		if(-1 == send(_sockfd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
		{
			cerr<<"send fail;errno:"<<errno<<endl;
			return;
		}
	}
}
//注册功能方法
void Client::register_func()
{
	cout<<"*注 册*"<<endl;
	start_login_register(0);//argv=0,register
	char buff[1024] = {0};
	if(recv(_sockfd,buff,1023,0) > 0)
	{
		cout<<buff<<endl;
	}
}

void Client:: one_to_one()
{
}

void Client::group_chat()
{

}

//登录功能方法
void Client::load_func()
{
	cout<<"*登 录*"<<endl;
	bool key = true;
	while(key)
	{
		start_login_register(1);//argv=1,login
		char msg[1024] = {0};
		if(recv(_sockfd,msg,1023,0) > 0)
		{
			cout<<msg<<endl;
		}
		if(strstr(msg,"在线") != NULL)
		{
			return;
		}
		if(strstr(msg,"失败") != 0)
		{
			continue;
		}
		else
		{
			chat_show();//登录成功后的视图
			key = false;
		}
	}
	bool flag = true;
	while(flag)
	{
		cout<<"请选择功能:"<<endl;
		char choice[2] = {0};
		cin>>choice;
		fflush(stdin);
		if(strlen(choice) > 1)
		{
			continue;
		}
		if(choice[0] > '9' || choice[0] < '0')
		{
			continue;
		}
		int num = atoi(&choice[0]);
		switch(num)
		{
			case 1:one_to_one();
				   flag = false;
				   break;
			case 2:group_chat();
				   flag = false;
				   break;
			case 3:
				   flag = false;
				   break;
			default:
				   flag = true;
		}
		memset(choice,0,sizeof(choice));
	}
}

static int sockfd;
void fun(int sig)
{
	cout<<"系统意外终止!"<<endl;
	Json::Value val;
	val["TYPE"] = MSG_TYPE_EXIT;
	send(sockfd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0);
	close(sockfd);
	cout<<"退出成功!"<<endl;
	exit(1);
}

//退出功能方法
void Client::exit_func()
{
	cout<<"*退 出*"<<endl;
	sockfd = _sockfd;
	Json::Value val;
	val["TYPE"] = MSG_TYPE_EXIT;
	send(sockfd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0);
	close(sockfd);
	cout<<"退出成功!"<<endl;
	exit(1);
}

void Client::run()
{
	//创建第二个线程，用于监听服务器消息
	//此处调用视图完成功能呈现
	function();
	signal(SIGINT,fun);//客户端意外终止
	signal(SIGQUIT,fun);//与上面情况相同
	bool flag = true;
	while(flag)
	{
		cout<<"请选择功能:"<<endl;
		char choice[2] = {0};
		cin>>choice;
		fflush(stdin);
		if(strlen(choice) > 1)
		{
			continue;
		}
		if(choice[0] > '9' || choice[0] < '0')
		{
			continue;
		}
		int num = atoi(&choice[0]);
		
		switch(num)
		{
			case 1://register
					register_func();
					flag = false;
					break;
			case 2://load
					load_func();
					flag = false;
					break;
			case 3://exit
					exit_func();
					flag = false;
					break;
			dafult:
				flag = true;
		}
		memset(choice,0,sizeof(choice));
	}
}
