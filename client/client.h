#ifndef _CLIENT_H
#define _CLIENT_H
#include<iostream>
#include<json/json.h>
#include<string>
#include<pthread.h>
using namespace std;

class Client
{
	public:
		Client(char*ip,unsigned short port);
		~Client();
		void start_login_register(int flag);
		void run();
		void register_func();
		void load_func();
		void exit_func();
		void one_to_one();
		void group_chat();
		friend void fun(int sig);
	private:
		friend void* p_fun(void*arg);
		int _sockfd;
	//	pthread_mutex_t mutex;
		pthread_t id;
		string regist_str;
		string login_str;
};


#endif
