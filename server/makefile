all: ser
ser: mpthread.o  contral.o main.o tcpsever.o view_login.o view_register.o view_exit.o view_talk_one.o
	g++ -o ser *.o -levent -ljson -lpthread -lmysqlclient -g
mpthread.o:mpthread.cpp
	g++ -c mpthread.cpp -g
contral.o:contral.cpp
	g++ -c contral.cpp -g
main.o:main.cpp
	g++ -c main.cpp -g
tcpsever.o:tcpsever.cpp
	g++ -c tcpsever.cpp -g
view_login.o:view_login.cpp
	g++ -c view_login.cpp -g
view_register.o:view_register.cpp
	g++ -c view_register.cpp -g
view_exit.o:view_exit.cpp
	g++ -c view_exit.cpp -g
view_talk_one.o:view_talk_one.cpp
	g++ -c view_talk_one.cpp
clean:
	rm -rf *.o
