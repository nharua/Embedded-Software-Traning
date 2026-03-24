CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = channel_app.out
TEST_TARGET = test_app.out

SRCS = main.c hw_model.c channel_manager.c command_parser.c
OBJS = $(SRCS:.c=.o)

TEST_SRCS = test_channel_manager.c hw_model.c channel_manager.c command_parser.c
TEST_OBJS = $(TEST_SRCS:.c=.o)

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
