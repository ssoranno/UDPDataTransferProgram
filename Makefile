CC= /usr/bin/gcc
all:	udpclient udpserver

udpclient: udpclient.c proj2common.c;
	${CC} -o  udpclient udpclient.c proj2common.c

udpserver: udpserver.c proj2common.c;
	${CC} -o udpserver udpserver.c proj2common.c -lm

clean:
	rm udpclient udpserver
