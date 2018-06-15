#include<iostream>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<pthread.h>
#include"tcpsever.h"
#include<event.h>
#include<unistd.h>
#include<functional>
#include<algorithm>
#include<time.h>
#include"mpthread.h"
using namespace std;

Sockpair::Sockpair(int sockpair0,int sockpair1):_sockpair0(sockpair0),_sockpair1(sockpair1){}

Tcpsever::Tcpsever(char *ip,unsigned short port,int pth_num)
{
	//创建服务器
	int listenfd = socket(AF_INET,SOCK_STREAM,0);
	if(listenfd == -1)
	{
		cout<<"socket err"<<endl;
		return;
	}
	struct sockaddr_in saddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip);

	int res = bind(listenfd,(struct sockaddr*)&saddr,sizeof(saddr));
	if(res == -1)
	{
		return;
	}
	listen(listenfd,5);
	_listen_fd = listenfd;

	_pth_num = pth_num;

	//初始化libevent
	_base = event_base_new();
}


void Tcpsever::create_pth()
{
	for(int i=0;i<_pth_num;++i)
	{
		new Mpthread(_sockpair_base[i]._sockpair1);//??bug
	}
}
int _count = 0;//client num
void listen_cb(int fd,short event,void*arg)
{
	Ptcpsever mthis = (Ptcpsever)arg;
	int listen_fd = mthis->_listen_fd;
	struct sockaddr_in caddr;
	int len = sizeof(caddr);
	static clock_t start = clock();//start time
	//accept
	int cli_fd = accept(listen_fd,(struct sockaddr*)&caddr,(socklen_t*)&len);
	if(cli_fd < 0 )
	{
		return;
	}
	_count++;
	cout<<_count<<" "<<cli_fd<<endl;
	clock_t end = clock();
	cout<<"time is :"<<(double)(end-start)/CLOCKS_PER_SEC<<"s"<<endl;
	//map.find不是使用find方法
	map<int,int>::iterator it = mthis->_pth_num_map.begin();
	int num = it->second;
	map<int,int>::iterator tmp = it;
	for(;it != mthis->_pth_num_map.end();++it)
	{
		cout<<it->second<<" ";
		if(it->second < num)
		{
			num = it->second;
			tmp = it;
		}
	}
	cout<<endl;
	//send(map.first   cli_fd);
	send(tmp->first,&cli_fd,sizeof(int),0);
}

void Tcpsever::run()
{
	//创建socketpair
	create_socket_pair();

	create_pth();

	//将监听套接子libevent -> listen_cb()
	struct event* ev = event_new(_base,_listen_fd,EV_READ|EV_PERSIST,listen_cb,(void*)this);
	event_add(ev,NULL);

	//循环监听
	event_base_dispatch(_base);
}



void sock_0_cb(int fd,short event,void* arg)
{

	//强转参数
	map<int,int> *_pth_num_map = (map<int,int>*)arg;

	//recv   当前对应线程的监听量num
	int num = 0;
	if((recv(fd,&num,sizeof(int),0)) == -1)
	{
		cerr<<"recv error"<<endl;
		return;
	}
	//更新到map表
	(*_pth_num_map)[fd] = num;
}

void Tcpsever::create_socket_pair()
{
	//申请
	for(int i = 0; i < _pth_num;i++)
	{
		//socketpair(int [2])
		int _pipe[2];
		if((socketpair(AF_UNIX,SOCK_STREAM,0,_pipe)) == -1)
		{
			cerr<<"socketpair error"<<endl;
			return;
		}
		//_socket_pair_base.push_back()
		_sockpair_base.push_back(Sockpair(_pipe[0],_pipe[1]));
		//_pth_num_map.push_buck(make_pair(socketpair0,0))
		_pth_num_map.insert(make_pair(_pipe[0],0));

		//将socketpair0端加入到libevent
		struct event *ev = event_new(_base,_pipe[0],EV_READ | EV_PERSIST,sock_0_cb,(void*)&_pth_num_map);

		event_add(ev,NULL);
		//启动循环监听  
//		event_base_dispatch(_base);
	}
}
