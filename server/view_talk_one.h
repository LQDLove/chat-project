#ifndef VIEW_TALK_ONE_H
#define VIEW_TALK_ONE_H
#include<string>
#include"view.h"
using namespace std;
class view_talk_one : public  view
{
	public:
		void process(Json::Value val,int cli_fd);
		void responce();
	private:
		string reason;
		int _cli_fd;
};
#endif
