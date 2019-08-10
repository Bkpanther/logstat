CFLAGS = -Wall -O3 -std=c++14
HEADERS = -I ./include/logstat
CC = g++
SOURCES = $(shell find . -name "*.cc")
BIN = $(shell mkdir -p bin)
EXEC = bin/logstat

all: bin logstat

bin: $(BIN)

logstat:
	$(CC) $(CFLAGS) $(HEADERS) -o $(EXEC) $(SOURCES)

.PHONY: clean

clean:
	rm -f $(EXEC)