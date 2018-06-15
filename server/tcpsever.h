#ifndef _TCPSEVER_H_
#define _TCPSEVER_H_
#include<iostream>
#include<vector>
#include<map>
#include<memory.h>
using namespace std;
#define MAX 20
class Sockpair
{
public:	
	Sockpair(int sockpair0,int sockpair1);
	int _sockpair0;
	int _sockpair1;
};

typedef class Tcpsever
{
	public:
		Tcpsever(char *ip,unsigned short port,int pth_num);
		~Tcpsever(){}
		void run();
		void create_socket_pair();
		void create_pth();

	private:
		struct event_base* _base;//libevent
		int _listen_fd;//listenfd
		int _pth_num;//线程个数
		//vector<int[2]> _sockpair_base;//socketpair
		vector<Sockpair> _sockpair_base;
		map<int,int> _pth_num_map;//线程监听数量的map表
		friend void sock_0_cb(int fd,short event,void* arg);
		friend void listen_cb(int fd,short event,void* arg);
}Tcpsever,*Ptcpsever;



#endif
