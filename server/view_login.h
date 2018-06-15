#ifndef VIEW_LOGIN_H
#define VIEW_LOGIN_H
#include"view.h"
#include<string>
using namespace std;
class view_login : public  view
{
	public:
		void process(Json::Value val,int cli_fd);
		void responce();
	private:
		string reason;
		int _cli_fd;
};
#endif
