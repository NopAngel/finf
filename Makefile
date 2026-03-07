CC = gcc
SOURCE = disk.c
TARGET = finfdisk
CFLAGS =

all:
	$(CC) $(SOURCE) -o $(TARGET) $(CFLAGS)
	./$(TARGET) --t
