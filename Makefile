OBJS_SERVER = server.o
OBJS_CLIENT = client.o
CC = c++
CFLAGS = -g -Wall -Wextra -Werror

all: ncserver ncclient

ncserver: $(OBJS_SERVER)
	$(CC) $(CFLAGS) -o ncserver $(OBJS_SERVER)

ncclient: $(OBJS_CLIENT)
	$(CC) $(CFLAGS) -o ncclient $(OBJS_CLIENT)

clean:
	rm -f *.o

fclean:clean
	rm -f ncclient ncserver

re: fclean all