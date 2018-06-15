#include<iostream>
#include "contral.h"
#include "view_login.h"
#include "public.h"
#include "view_register.h"
#include "view_exit.h"
#include "view_talk_one.h"
#include "view.h"
#include<errno.h>
#include<map>

using namespace std;

contral:: contral()
{
	_map.insert(make_pair(MSG_TYPE_REGISTER,new view_register()));//register
	_map.insert(make_pair(MSG_TYPE_LOGIN,new view_login()));//login
	_map.insert(make_pair(MSG_TYPE_EXIT,new view_exit()));// exit
	_map.insert(make_pair(MSG_TYPE_TALK_ONE,new view_talk_one()));//1 v 1
}

void contral::process(char*buff,int cli_fd)
{
	//json解析 -》TYPE
	Json::Value val;
	Json::Reader read;
//	cout<<buff<<endl;
	if(-1 == read.parse(buff,val))
	{
		cerr<<"read fail;error:"<<errno<<endl;
		return;
	}
//	cout<<"_map info:"<<_map->size()<<endl;
//	cout<<"before _map.find() "<<endl;
	//_map[val["TYPE"].asInt()]->process(val,cli_fd);
	map<int,view*>::iterator it = _map.find(val["TYPE"].asInt());
	if(it != _map.end())
	{
		it->second->process(val,cli_fd);
	//	it->second->responce();
	}
	else
	{
		cerr<<"find fail view;errno:"<<errno<<endl;
		return ;
	}
}
