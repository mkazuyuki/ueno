TARGET	= heartbeat
CFLAGS = -Wall -g


all: $(TARGET)

$(TARGET): heartbeat.c
	$(CC) -o $@ $<

clean:
	rm -f $(TARGET) *~
