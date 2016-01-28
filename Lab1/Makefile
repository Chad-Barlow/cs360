# Makefile for echo client and server

CXX=			g++ $(CCFLAGS)

CLIENT=			download.o
OBJS =			$(ECHO-CLIENT)

LIBS=

CCFLAGS= -g

all:	client

client:$(CLIENT)
	$(CXX) -o download $(CLIENT) $(LIBS)

clean:
	rm -f $(OBJS) $(OBJS:.o=.d)

realclean:
	rm -f $(OBJS) $(OBJS:.o=.d) download


# These lines ensure that dependencies are handled automatically.
%.d:	%.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
		| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
		[ -s $@ ] || rm -f $@'

include	$(OBJS:.o=.d)
