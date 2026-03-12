CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = main.out
SRCS = main.c

all: $(TARGET)
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET)

