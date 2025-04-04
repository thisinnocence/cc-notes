all: send.out recv.out

send.out: send_ping.c
	gcc -g $^ -o $@

recv.out: recv_ping.c
	gcc -g $^ -o $@

clean:
	rm -fv *.out
