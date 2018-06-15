#include<iostream>
#include<string>
#include<json/json.h>
#include<errno.h>
#include "view_login.h"
#include<mysql/mysql.h>
#include<string.h>
#include<stdio.h>
#include"view.h"
#include<sys/socket.h>
using namespace std;

void view_login::process(Json::Value val,int cli_fd)
{
	_cli_fd = cli_fd;
	MYSQL *mpcon = mysql_init((MYSQL*)0);
	MYSQL_RES *mp_res;
	MYSQL_ROW mp_row;
	//连接mysql
	if(!mysql_real_connect(mpcon,"127.0.0.1","root","123456",NULL,3306,NULL,0))
	{
		cerr<<"sql connect fail;errno:"<<errno<<endl;
		return;
	}
	//连接项目数据库
	if(mysql_select_db(mpcon,"myQQ"))
	{
		cerr<<"select fail: errno"<<errno<<endl;
		return;
	}
	//拼接mysql操作语句--查询usr表
	char name[128]={0};
	strcpy(name,val["name"].asString().c_str());
	char cmd[256] = "select * from usr where name='";
	char buff[3] = "';";
	strcat(cmd,name);
	strcat(cmd,buff);
	//访问usr表
	if(mysql_real_query(mpcon,cmd,strlen(cmd)))
	{
		cerr<<"usr fail;errno:"<<errno<<endl;
		return ;
	}
	mp_res = mysql_store_result(mpcon);
	mp_row = mysql_fetch_row(mp_res);
	reason.clear();//清空reason
	if(mp_row == 0)
	{
		//err usrname pw
		char msg[] = " 登录失败，用户名无效，请重新输入!";
		send(_cli_fd,msg,strlen(msg),0);
		return;
	}
	if(strcmp(mp_row[1],val["pw"].asString().c_str()) != 0)
	{
		char msg[] = "登录失败,密码错误，请重新输入!";
		send(_cli_fd,msg,strlen(msg),0);
		return ;
	}
	
	//访问online表
	char cmd_on_sel[256] = "select * from online where name='";//先查询是否有同名用户在线
	strcat(cmd_on_sel,name);
	strcat(cmd_on_sel,buff);
	if(mysql_real_query(mpcon,cmd_on_sel,strlen(cmd_on_sel)))
	{
		cerr<<"online fail;errno:"<<errno<<endl;
		return ;
	}
	mp_res = mysql_store_result(mpcon);
	mp_row = mysql_fetch_row(mp_res);
	if(mp_row != 0)//同名登录情况
	{
		char msg[]="登录失败,该用户已在线!";
		send(_cli_fd,msg,strlen(msg),0);
		return ;
	}
	
	char cmd_on[256] = "insert into online values('";//再把用户插入到online中
	char str[2] = {0};
	sprintf(str,"%d",cli_fd);
	strcat(cmd_on,str);
	strcat(cmd_on,"','");
	strcat(cmd_on,name);
	strcat(cmd_on,"');");
	if(mysql_real_query(mpcon,cmd_on,strlen(cmd_on)))
	{
		cerr<<"online fail;errno:"<<errno<<endl;
		return ;
	}
	char msg[]="登录成功!";
	send(_cli_fd,msg,strlen(msg),0);
	
	//访问offline表
	//拼接mysql操作语句--查询offline表
	bool finish  = false;//offline表中没有离线消息时，置true
	char cmd_off[256] = "select * from offline where name='";
	strcat(cmd_off,name);
	strcat(cmd_off,"';");
	
	if(mysql_real_query(mpcon,cmd_off,strlen(cmd_off)))
	{
		cerr<<"name query fail;errno:"<<errno<<endl;
		return ;
	}
	mp_res = mysql_store_result(mpcon);
	while(!finish)
	{
		mp_row = mysql_fetch_row(mp_res);
		if(mp_row != 0)
		{
			//将离线消息发送给该用户
			char msg[256] = {0};
			strcpy(msg,mp_row[1]);
			send(_cli_fd,msg,strlen(msg),0);
			//从离线表中删除离线消息和离线用户的信息
		}
		else
		{
			finish = true;
			char cmd_del[256] = "delete from offline where name='";
			strcat(cmd_del,name);
			strcat(cmd_del,"';");
			if(mysql_real_query(mpcon,cmd_del,strlen(cmd_del)))
			{
				cerr<<"name query fail;errno:"<<errno<<endl;
				return ;
			}
		}
	}
}

void view_login::responce()
{
//	send(_cli_fd,&reason,reason.length(),0);//string是从json中解析出来的数据
}
