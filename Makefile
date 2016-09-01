CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-g $(shell root-config --cflags)
LDFLAGS=-g $(shell root-config --ldflags)
LDLIBS=-lcurl $(shell root-config --libs)

SRCS=ftpclient.cpp
OBJS=$(subst .cc,.o,$(SRCS))

all: ftpclient

ftpclient: $(OBJS)
	$(CXX) $(LDFLAGS) -o ftpclient $(OBJS) $(LDLIBS) 

#ftpclient.o: ftpclient.cpp

%.o: %.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) ftpclient