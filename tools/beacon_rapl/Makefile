optkit_beacon: optkit_beacon_rapl.cc
	g++ optkit_beacon_rapl.cc -o optkit_beacon_rapl -I/usr/local/include/optkit/ -I/usr/local/include/spdlog -lOptimizerToolkitShared -lspdlog -lpfm -ldl -lpthread

run: optkit_beacon_rapl
	./optkit_beacon_rapl 10

clean:
	rm ./optkit_beacon_rapl