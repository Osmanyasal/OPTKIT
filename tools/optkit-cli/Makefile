all: optkit

optkit: optkit.cc
	g++ -o optkit \
	optkit.cc \
    ../../bin/Release/liboptkit_static.a \
    -I../../src/ \
    -I../../lib/spdlog/include \
    -I../../lib/libpfm4/include \
    -ldl -lpthread

clean:
	rm -f optkit