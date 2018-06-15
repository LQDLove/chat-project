#include<iostream>
#include<string>
#include<json/json.h>
#include<errno.h>
#include<mysql/mysql.h>
#include<string.h>
#include<sys/socket.h>
#include<stdio.h>
#include"view.h"
#include"view_exit.h"
using namespace std;

void view_exit::process(Json::Value val,int cli_fd)
{
	cout<<cli_fd<<" client over!"<<endl;
	_cli_fd = cli_fd;
	MYSQL*mpcon = mysql_init((MYSQL*)0);
	MYSQL_RES*mp_res;
	MYSQL_ROW*mp_row;
	if(!mysql_real_connect(mpcon,"127.0.0.1","root","123456",NULL,3306,NULL,0))
	{
		cerr<<"sql connect fail;errno:"<<errno<<endl;
		return;
	}
	if(mysql_select_db(mpcon,"myQQ"))
	{
		cerr<<"select fail;errno:"<<errno<<endl;
		return;
	}
	char cmd[256] = "delete from online where fd='";
	char buff[5] = {0};
	sprintf(buff,"%d",_cli_fd);
	strcat(cmd,buff);
	strcat(cmd,"';");
	if(mysql_real_query(mpcon,cmd,strlen(cmd)))
	{
		cerr<<"query fail;errno:"<<errno<<endl;
		return;
	}
	close(cli_fd);
}

void view_exit::responce()
{
}
