CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = fsh
SRCS = fsh.c execute.c parser.c prompt.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)