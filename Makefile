CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-g $(shell root-config --cflags)
LDFLAGS=-g $(shell root-config --ldflags)
LDLIBS=-lcurl $(shell root-config --libs)

SRCS=ftpclient.cpp logger.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: ftpclient

ftpclient: $(OBJS)
	$(CXX) $(LDFLAGS) -o ftpclient $(OBJS) $(LDLIBS) 

ftpclient.o: ftpclient.cpp

logger.o: logger.cpp

#%.o: %.cpp
#	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

run: all
	./ftpclient

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) ftpclient
