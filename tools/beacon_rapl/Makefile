all: optkit_beacon

optkit_beacon: optkit_beacon_rapl.cc
	g++ -o optkit_beacon_rapl \
	optkit_beacon_rapl.cc \
    ../../bin/Release/liboptkit_static.a \
    -I../../src/ \
    -I../../lib/libpfm4/include \
    -I../../lib/spdlog/include \
    -L../../lib/spdlog/build \
    -L../../lib/libpfm4/lib \
    -lspdlog -lpfm -ldl -lpthread
	
run: optkit_beacon_rapl
	./optkit_beacon_rapl 10

clean:
	rm ./optkit_beacon_rapl