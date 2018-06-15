all: cli
cli: client_update.o  view_cli.o cli_main.o 
	g++ -o cli client_update.o view_cli.o cli_main.o -ljson
client_update.o:client_update.cpp
	g++ -c client_update.cpp
view_cli.o:view_cli.cpp
	g++ -c view_cli.cpp
cli_main.o:cli_main.cpp
	g++ -c cli_main.cpp
clean:
	rm -rf *.o
