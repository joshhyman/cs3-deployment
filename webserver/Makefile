CC = g++
CXX = g++
CPPFLAGS = -MMD
CXXFLAGS = -Wall -pedantic -g -std=c++11
LDLIBS = -lboost_system -lpthread
SRCS = config_parser.cc handler.cc hwHandler.cc server_main.cc handlerList.cc server.cc sfHandler.cc socketWrapper.cc

server_main: server_main.o config_parser.o handlerList.o hwHandler.o handler.o sfHandler.o socketWrapper.o server.o

clean:
	rm -f server_main *.o *~

.PHONY: all clean

-include $(SRCS:.cc=.d)
