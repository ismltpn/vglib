default: demo

all: demo to_eps append

demo: vglib.o demo.c
	gcc -o demo vglib.o demo.c -lm
	./demo

to_eps: vglib.o to_eps.c
	gcc -o to_eps vglib.o to_eps.c -lm

append: vglib.o append.c
	gcc -o append vglib.o append.c -lm

vglib.o: vglib.h vglib.c
	gcc -c vglib.c

cleanall: cleanbin clean

clean:
	rm -f *.eps
	rm -f *.svg

cleanbin:
	rm -f demo
	rm -f append
	rm -f to_eps
	rm -f *.o
	rm -f *.out
