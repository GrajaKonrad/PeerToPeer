all: build link

build:
	g++ -c src/EntryPoint.cpp -o bin-int/EntryPoint.o
	g++ -c src/Application.cpp -o bin-int/Application.o
	g++ -c src/Server.cpp -o bin-int/Server.o
	g++ -c src/Client.cpp -o bin-int/Client.o
	g++ -c src/Message.cpp -o bin-int/Message.o
	g++ -c src/Command.cpp -o bin-int/Command.o

link:
	g++ -o bin/p2p -Wall -pthread \
		bin-int/EntryPoint.o \
		bin-int/Application.o \
		bin-int/Server.o \
		bin-int/Client.o \
		bin-int/Message.o \
		bin-int/Command.o
