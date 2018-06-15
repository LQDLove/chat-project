#ifndef VIEW_REGISTER_H
#define VIEW_REGISTER_H
#include<string>
#include "view.h"
using namespace std;
class view_register : public view
{
	public:
		void process(Json::Value val,int cli_fd);
		void responce();
	private:
		string reason;
		int _cli_fd;
};

#endif
