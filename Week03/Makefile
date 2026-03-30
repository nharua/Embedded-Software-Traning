CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = soduku_app.out

SRCS = main.c 
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_OBJS)

clean:
	rm -f $(TARGET) $(TEST_TARGET) *.o

run: all
	./$(TARGET)
