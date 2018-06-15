#include<iostream>
#include<string>
#include<stdio.h>
#include<string.h>
#include<mysql/mysql.h>
using namespace std;

int main()
{
	cout<<"mpcon st"<<endl;
	MYSQL *mpcon = mysql_init((MYSQL*)0);
	cout<<"mpcon end"<<endl;
	MYSQL_RES *mp_res;
	MYSQL_ROW mp_row;
	if(!mysql_real_connect(mpcon,"127.0.0.1","root","123456",NULL,3306,NULL,0))
	{
		cout<<"connect fail"<<endl;
		return -1;
	}
	if(mysql_select_db(mpcon,"myQQ"))
	{
		cout<<"select db fail"<<endl;
		return -1;
	}
	for(int i=0;i<10000;++i)
	{
		char buff[20] = "name_";
		char tmp[5] = {0};
		sprintf(tmp,"%d",i);
		strcat(buff,tmp);
		char cmd[256] = "insert into usr values('";
		strcat(cmd,buff);
		strcat(cmd,"','");
		strcat(cmd,"**");
		strcat(cmd,"');");
		cout<<cmd<<endl;
		if(mysql_real_query(mpcon,cmd,strlen(cmd)))
		{
			cout<<"insert fail"<<endl;
			return -1;
		}
	}
	return 0;
}
