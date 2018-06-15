#include<iostream>
#include<string>
#include<json/json.h>
#include<errno.h>
#include<mysql/mysql.h>
#include<string.h>
#include<sys/socket.h>
#include"view.h"
#include"view_register.h"
using namespace std;

void view_register::process(Json::Value val,int cli_fd)
{
//	cout<<"view_register::process"<<endl;
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
//	cout<<"database connect succ!"<<endl;
	//连接项目数据库
	if(mysql_select_db(mpcon,"myQQ"))
	{
		cerr<<"select fail: errno"<<errno<<endl;
		return;
	}
	//先查询usr表，如果存在与当前要注册的用户名一致，提醒用户重新输入用户名；否则
	//将当前用户信息插入到usr表中，并回复注册成功，提醒登录
//	cout<<"myQQ connect succ!"<<endl;
	//拼接mysql语句--查询usr表
	char name[128]={0};
	strcpy(name,val["name"].asString().c_str());
	char cmd[256] = "select * from usr where name='";
	char buff[3] = "';";
	strcat(cmd,name);
	strcat(cmd,buff);
//	cout<<"start usr"<<endl;
	//访问usr表
	if(mysql_real_query(mpcon,cmd,strlen(cmd)))
	{
		cerr<<"name query fail;errno:"<<errno<<endl;
		return;
	}
	mp_res = mysql_store_result(mpcon);
	mp_row = mysql_fetch_row(mp_res);
	if(mp_row != 0)
	{
//		cout<<"mp_row != 0"<<endl;
//		reason.clear();//删除上一次操作在reason中存放的数据，避免对本次操作产生影响
//		reason.insert(reason.size(),"该用户名已使用过，请重新输入!\n");
//		cout<<"insert"<<endl;
		char msg[] = "注册失败!";
		send(_cli_fd,msg,strlen(msg),0);
		return;
	}
//	cout<<"start register new user"<<endl;
	//开始注册新用户，即往usr表中插入新用户信息
	char cmd_insert[128] = "insert into usr values('";
	char pw[16] = {0};
	strcpy(pw,val["pw"].asString().c_str());
	strcat(cmd_insert,name);
	strcat(cmd_insert,"','");
	strcat(cmd_insert,pw);
	strcat(cmd_insert,"');");
	if(mysql_real_query(mpcon,cmd_insert,strlen(cmd_insert)))
	{
		cerr<<"insert query fail;errno:"<<errno<<endl;
		return;
	}
	//插入操作成功，向用户发送注册成功的回复
	char msg[] = "注册成功!";
	send(_cli_fd,msg,strlen(msg),0);
}

void view_register::responce()
{
}
