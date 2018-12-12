all: main control

main: main.o
	gcc -o main main.o

control: control.o
	gcc -o control control.o

main.o: main.c
	gcc -c main.c

control.o: control.c
	gcc -c control.c

clean:
	rm main control main.o control.o
	
run:
	./main

setup:
	./control $(args)
