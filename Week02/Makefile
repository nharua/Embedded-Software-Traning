CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = channel_app.out
SRCS = main.c hw_model.c channel_manager.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

test: all
	./$(TARGET) test

run: all
	./$(TARGET)

