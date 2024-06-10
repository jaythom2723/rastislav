CC=gcc
CFLAGS=-I./console/include
LDFLAGS=-lgdi32

all: console

console: $(wildcard ./console/src/*.c)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o console.exe

run: ./console.exe
	.\$^
