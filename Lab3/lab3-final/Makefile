# Makefile for echo client and server

CXX=			g++ $(CCFLAGS)

SERVER=			server.o 
OBJS =			$(SERVER)

LIBS=

CCFLAGS= -g -pthread

all:	server

server:$(SERVER)
	$(CXX) -o server $(SERVER) $(LIBS)

clean:
	rm -f $(OBJS) $(OBJS:.o=.d)

realclean:
	rm -f $(OBJS) $(OBJS:.o=.d) server


# These lines ensure that dependencies are handled automatically.
%.d:	%.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
		| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
		[ -s $@ ] || rm -f $@'

include	$(OBJS:.o=.d)
