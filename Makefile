OBJS_SERVER = server.o
CC = cc
CFLAGS = -g -Wall -Wextra -Werror

all: ncserver 

ncserver: $(OBJS_SERVER)
	$(CC) $(CFLAGS) -o ncserver $(OBJS_SERVER)

clean:
	rm -f *.o

fclean:clean
	rm -f ncserver

re: fclean all