all: optkit_probe

optkit_probe: probe.cc
	g++ -o optkit_probe \
	probe.cc \
    ../../bin/Release/liboptkit_static.a \
    ../../lib/libpfm4/lib/libpfm.a\
    -I../../src/ \
    -I../../lib/libpfm4/include \
    -ldl -lpthread

run: optkit_probe
	./optkit_probe

clean:
	rm -f optkit_probe