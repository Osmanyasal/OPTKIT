all: optkit_probe

optkit_probe: probe.cc
	g++ -o optkit_probe \
	probe.cc \
    ../../bin/Release/liboptkit_static.a \
    -I../../src/ \
    -I../../lib/libpfm4/include \
    -I../../lib/spdlog/include \
    -L../../lib/spdlog/build \
    -L../../lib/libpfm4/lib \
    -lspdlog -lpfm -ldl -lpthread

run: optkit_probe
	./optkit_probe

clean:
	rm -f optkit_probe