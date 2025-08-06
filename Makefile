OBJS_SERVER = server.o
OBJS_CLIENT = client.o
CC = gcc
CFLAGS = -g -Wall

all: ncserver ncclient

ncserver: $(OBJS_SERVER)
	$(CC) $(CFLAGS) -o ncserver $(OBJS_SERVER)

ncclient: $(OBJS_CLIENT)
	$(CC) $(CFLAGS) -o ncclient $(OBJS_CLIENT)

server.o: server.c
	$(CC) $(CFLAGS) -c server.c

client.o: client.c
	$(CC) $(CFLAGS) -c client.c

clean:
	rm -f *.o ncserver ncclient

