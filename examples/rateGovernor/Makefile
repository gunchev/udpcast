CFLAGS = -I../.. -fPIC -Wall

ipe.so: ipe.o
	gcc -shared -o ipe.so ipe.o

clean:
	rm -f ipe.o ipe.so
