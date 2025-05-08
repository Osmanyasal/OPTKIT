all: optkit_io_interceptor.so

optkit_io_interceptor.so: optkit_io_interceptor.o
	g++ -shared -fPIC -o optkit_io_interceptor.so optkit_io_interceptor.o -ldl

optkit_io_interceptor.o: optkit_io_interceptor.cc
	g++ -fPIC -D_GNU_SOURCE -c optkit_io_interceptor.cc -o optkit_io_interceptor.o

clean:
	rm -f *.o *.so