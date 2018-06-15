#ifndef MPTHREAD_H
#define MPTHREAD_H
#include "contral.h"
#include<iostream>
#include<map>
#include<event.h>
using namespace std;
typedef class mpthread
{
	public:
		mpthread(int sock_1);
		~mpthread(){}
	private:
		int _sock_1;//socketpair1
		struct event_base* _base;//libevent
		map<int,struct event*> _event_map;//保存事件的map
		Contral *Mcontral;//控制台
		friend void *pth_run(void*arg);
		friend void sock_1_cb(int fd,short event,void *arg);
		friend void cli_cb(int fd,short event,void *arg);
}Mpthread,*Pmpthread;

#endif
