all: optkit_beacon

optkit_beacon: optkit_beacon_rapl.cc
	g++ -o optkit_beacon_rapl \
	optkit_beacon_rapl.cc \
    ../../bin/Release/liboptkit_static.a \
    ../../lib/libpfm4/lib/libpfm.a\
    -I../../src/ \
    -I../../lib/libpfm4/include \
    -ldl -lpthread
	
run: optkit_beacon_rapl
	./optkit_beacon_rapl 10

clean:
	rm ./optkit_beacon_rapl