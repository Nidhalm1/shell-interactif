CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lreadline
TARGET = fsh
SRCS = fsh.c execute.c parser.c prompt.c command.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET)