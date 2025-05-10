optkit_beacon: optkit_beacon_rapl.cc
	g++ optkit_beacon_rapl.cc -o optkit_beacon_rapl -I/../../src/ -I../../lib/libpfm4/include -I../../lib/spdlog/include -L../../lib/spdlog/build -L../../lib/libpfm4/lib  -L../../bin/Release/ -lOPTKIT.so -lspdlog -lpfm -ldl -lpthread

run: optkit_beacon_rapl
	./optkit_beacon_rapl 10

clean:
	rm ./optkit_beacon_rapl