#include<iostream>
#include<pthread.h>
#include "mpthread.h"
#include <event.h>
#include"json/json.h"
#include "view_exit.h"
using namespace std;

void *pth_run(void *arg);

Mpthread::mpthread(int sock_1)
{
	Mcontral = new Contral;
	_sock_1 = sock_1;
	//启动子线程
	pthread_t id;
	if(pthread_create(&id,NULL,pth_run,this) != 0)
	{
		cerr<<"pthread_create err"<<endl;
		return;
	}
//	cout<<_sock_1<<endl;
	//->pth_run(this)
}
void cli_cb(int fd,short event,void *arg)
{
	Pmpthread mthis = (Pmpthread)arg;
	//recv ->buff
	char buff[1024] = {0};
	int recv_num = recv(fd,buff,sizeof(buff)-1,0);
	if(recv_num > 0)//从客户端可以接受到数据
	{
	//buff -> contral
		mthis->Mcontral->process(buff,fd);
	}
//	event_del(&event);//从客户端读不到数据时，认为客户端关闭，从libevent中删除事件，并将该事件从线程的map表中删除
	if(recv_num <=0)
	{
		cout<<"客户端意外终止!"<<endl;
		event_del(mthis->_event_map[fd]);
	//	cout<<"event_del suc"<<endl;
		mthis->_event_map.erase(fd);
	//	cout<<"event_map.erase suc"<<endl;
		view_exit ex;
		Json::Value val;
		ex.process(val,fd);
		return;
	}
}

void sock_1_cb(int fd,short event,void *arg)
{
	//强转
	Pmpthread mthis = (Pmpthread)arg;
	//recv cli_fd
	int cli_fd;
	if(recv(fd,&cli_fd,sizeof(cli_fd),0) == -1)//xxx
	{
		cerr<<"recv cli_fd err"<<endl;
		return;
	}
	//将cli_fd 加入到libevent ->cli_cb()
	struct event* ev = event_new(mthis->_base,cli_fd,EV_READ|EV_PERSIST,cli_cb,(void*)mthis);
	//event_add();
	event_add(ev,NULL);
	//将事件加入到_event_map
	mthis->_event_map.insert(make_pair(cli_fd,ev));
	int num = mthis->_event_map.size();
	send(fd,&num,sizeof(int),0);
}

void *pth_run(void*arg)
{
	Pmpthread mthis = (Pmpthread)arg;
	mthis ->_base = event_init();
	//将sock_1 加入到libevent ->sock_1_cb(,mthis,)
	struct event*ev = event_new(mthis->_base,mthis->_sock_1,EV_READ|EV_PERSIST,sock_1_cb,mthis);
//	cout<<"ev:"<<ev<<endl;
//	cout<<mthis->_sock_1<<endl;
	event_add(ev,NULL);
	event_base_dispatch(mthis->_base);
}

