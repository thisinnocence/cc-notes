.PHONY: all

all: aes.out aes_gcm.out aes_cbc.out aes_cmac.out aes_ctr.out

aes.out: aes.c
	gcc -g $^ -o $@ -lcrypto

aes_gcm.out: aes_gcm.c
	gcc -g $^ -o $@ -lcrypto

aes_cbc.out: aes_cbc.c
	gcc -g $^ -o $@ -lcrypto

aes_cmac.out: aes_cmac.c
	gcc -g $^ -o $@ -lcrypto

aes_ctr.out: aes_ctr.c
	gcc -g $^ -o $@ -lcrypto

clean:
	rm -f *.out
