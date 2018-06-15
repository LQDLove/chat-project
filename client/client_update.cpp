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
#include<pthread.h>
#include<sstream>
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
//	pthread_mutex_init(&mutex,NULL);//初始化互斥锁变量
}

Client:: ~Client()
{
	pthread_join(id,NULL);
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
static bool load_flag = false;//是否登录成功
//注册功能方法
void Client::register_func()
{
	cout<<"*注 册*"<<endl;
	start_login_register(0);//argv=0,register
	while(1)
	{
		if(regist_str.size() != 0)
		{
			cout<<regist_str<<endl;
			regist_str.clear();
			break;
		}
	}
}

void Client:: one_to_one()
{
	cout<<"*单人聊天*"<<endl;
	cout<<"请输入接收用户:"<<endl;
	fflush(stdout);
	char des_usr[20] ={0};
	cin>>des_usr;
	fflush(stdin);
	string msg;
//	char msg[1024] = {0};
//	char *msg = new char[1024];
	cout<<"请输入消息:"<<endl;
//	cin.get(msg,20);
//	cin>>msg;
	getline(cin,msg);
	getline(cin,msg);
//将目标用户和消息打包发送给服务器
	Json::Value val;
	val["TYPE"] = MSG_TYPE_TALK_ONE;
	val["name"] = des_usr;
	val["msg"] = msg;
	send(_sockfd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0);
	cout<<"发送成功!"<<endl;
//	delete []msg;
}

void Client::group_chat()
{
	cout<<"暂未上线"<<endl;
}

//登录功能方法
void Client::load_func()
{
	cout<<"*登 录*"<<endl;
	bool key = true;
	while(key)
	{
		start_login_register(1);//argv=1,login
		while(1)
		{
			if(login_str.size() != 0)
			{
				cout<<login_str<<endl;
				break;
			}
		}
		if(login_str.find("在线") != -1)
		{
			login_str.clear();
			return;
		}
		if(login_str.find("用户名无效") != -1)
		{
			login_str.clear();
			continue;
		}
		if(login_str.find("登录成功") != -1)
		{
			login_str.clear();
			chat_show();//登录成功后的视图
			load_flag = true;
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
	sleep(1);
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

void* p_fun(void*arg)//线程函数
{
	Client*mthis = (Client*)arg;
	char msg[1024] = {0};
	while(1)
	{
		int recv_num = recv(mthis->_sockfd,msg,1023,0);
		if(recv_num >0)
		{
			if(strstr(msg,"注册") != NULL)
			{
				mthis->regist_str = msg;
				memset(msg,0,sizeof(msg));
				continue;
			}
			if(strstr(msg,"登录") != NULL)
			{
				mthis->login_str = msg;
				memset(msg,0,sizeof(msg));
				continue;
			}
			if(strstr(msg,"新消息:") != NULL)
			{
				cout<<msg<<endl;
				memset(msg,0,sizeof(msg));
				continue;
			}
		}
		if(recv_num < 0)
		{
			//服务器断开连接，或者主线程断开连接
			break;
		}
	}
}


void Client::run()
{
	//创建第二个线程，用于监听从服务器接收消息
	pthread_create(&id,NULL,p_fun,this);
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
			case 3://chat
					if(load_flag == true)
					{
						one_to_one();
						flag = false;
						break;
					}
					else
					{
						cout<<"请先登录!"<<endl;
						continue;
					}
			case 4://exit
					exit_func();
					flag = false;
					break;
			dafult:
				flag = true;
		}
		memset(choice,0,sizeof(choice));
	}
}
