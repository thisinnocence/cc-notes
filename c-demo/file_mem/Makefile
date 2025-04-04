CC = gcc
CFLAGS = -Wall -Wextra

all: writer.out reader.out

writer.out: writer.c comm.h
	$(CC) $(CFLAGS) -o $@ $^

reader.out: reader.c comm.h
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f ipc_file *.out
