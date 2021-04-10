CC = gcc
CFLAGS = -Wall -I./include
OBJS = src/main.o src/bitBoard.o
TARGET = bin/rev

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDLIBS)

.PHONY: tmpclean clean

tmpclean:
	find . -name *~ -exec rm -vf {} \;
clean: tmpclean
	rm -f $(OBJS) $(TARGET)
