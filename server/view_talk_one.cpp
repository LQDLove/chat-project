#include<iostream>
#include<string>
#include<json/json.h>
#include<errno.h>
#include<mysql/mysql.h>
#include<string.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<stdio.h>
#include"view.h"
#include"view_talk_one.h"
using namespace std;

void view_talk_one::process(Json::Value val,int cli_fd)
{
	//检查要聊天的对象是否在线，如果不在线，发送离线消息
	_cli_fd = cli_fd;
	MYSQL* mpcon = mysql_init((MYSQL*)0);
	MYSQL_RES *mp_res;
	MYSQL_ROW mp_row;
	//连接数据库
	if(!mysql_real_connect(mpcon,"127.0.0.1","root","123456",NULL,3306,NULL,0))
	{
		cerr<<"sql connect fail;errno:"<<errno<<endl;
		return;
	}
	//连接项目数据库
	if(mysql_select_db(mpcon,"myQQ"))
	{
		cerr<<"select database fail;errno:"<<errno<<endl;
		return;
	}
	//拼接mysql操作语句--查询usr,检查是否有该用户
	char name[20] = {0};
	strcpy(name,val["name"].asString().c_str());
	char cmd[128] = "select * from usr where name='";
	strcat(cmd,name);
	strcat(cmd,"';");
	if(mysql_real_query(mpcon,cmd,strlen(cmd)))
	{
		cerr<<"usr fail;errno"<<errno<<endl;
		return;
	}
	mp_res = mysql_store_result(mpcon);
	mp_row = mysql_fetch_row(mp_res);
	if(mp_row == 0)
	{
		char msg[] = "新消息:无效用户,发送失败!";
		send(_cli_fd,msg,strlen(msg),0);
		return;
	}
	char cmd_online[128] = "select * from online where name='";
	strcat(cmd_online,name);
	strcat(cmd_online,"';");
	if(mysql_real_query(mpcon,cmd_online,strlen(cmd_online)))
	{
		cerr<<"online fail;errno"<<errno<<endl;
		return;
	}
	mp_res = mysql_store_result(mpcon);
	mp_row = mysql_fetch_row(mp_res);
	if(mp_row == 0)//usr not online
	{
		//获取当前用户的姓名作为发送人，编辑到离线消息中
		char cur_name[5] = {0};
		sprintf(cur_name,"%d",cli_fd);
		char cmd_f[128] = "select * from online where fd='";
		strcat(cmd_f,cur_name);
		strcat(cmd_f,"';");
		if(mysql_real_query(mpcon,cmd_f,strlen(cmd_f)))
		{
			cerr<<"cur_name fail;errno"<<errno<<endl;
			return;
		}
		mp_res = mysql_store_result(mpcon);
		mp_row = mysql_fetch_row(mp_res);
		if(mp_row != 0)
		{
			memset(cur_name,0,sizeof(cur_name));
			strcpy(cur_name,mp_row[1]);
		}
		char cmd_off[1024] = "insert ignore into offline(name,reason) values('";
		strcat(cmd_off,name);
		strcat(cmd_off,"','");
		strcat(cmd_off,"新消息:");
		strcat(cmd_off,val["msg"].toStyledString().c_str());
		strcat(cmd_off,"发送人:");
		strcat(cmd_off,cur_name);//??
		strcat(cmd_off,"\n");
		strcat(cmd_off,"');");
		if(mysql_real_query(mpcon,cmd_off,strlen(cmd_off)))		
		{
			cerr<<"offline fail;errno"<<errno<<endl;
			return;
		}
		char msg[128] = "新消息:用户不在线,已发送离线消息!";
		send(_cli_fd,msg,strlen(msg),0);
		return;
	}
	//在线，直接发送给该对象(从在线表中找出对象对应的fd，发送消息)
	int fd = atoi(mp_row[0]);
	char msg[1024] = "新消息:";
	strcat(msg,val["msg"].toStyledString().c_str());
	strcat(msg,"发送人:");
	strcat(msg,name);
	strcat(msg,"\n");
	send(fd,msg,strlen(msg),0);
}

void view_talk_one::responce()
{}
