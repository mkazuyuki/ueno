TARGET	= server client
SRCS 	= server.c client.c
OBJS 	= $(SRCS:.c=.o)
CC  	= gcc
LIBDIR  =
LIBS    =


# $(TARGET): $(OBJS)
# 	$(CC) -o $@ $^ $(LIBDIR)$(LIBS)

all: $(TARGET)

server: server.c 
	$(CC) -o $@ $^ $(LIBDIR)$(LIBS)

client: client.c 
	$(CC) -o $@ $^ $(LIBDIR)$(LIBS)

clean:
	-rm -f $(OBJS) $(TARGET) *.d