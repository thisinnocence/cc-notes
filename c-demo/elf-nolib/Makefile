LDFLAGS = -T linker.ld

.PHONY: all run clean
all: run a.out

run: a.out
	./a.out; echo "exit code: $$?"

a.out: foo.o
	ld $(LDFLAGS) $< -o $@

foo.o: foo.S
	as $< -o $@

clean:
	rm -f *.o *.out
