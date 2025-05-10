optkit_probe: probe.cc
	g++ probe.cc -o optkit_probe -I/../../src/ -I../../lib/libpfm4/include -I../../lib/spdlog/include -L../../lib/spdlog/build -L../../lib/libpfm4/lib  -L../../bin/Release/ -lOPTKIT.so -lspdlog -lpfm -ldl -lpthread

run: optkit_probe
	./optkit_probe

clean:
	rm -f optkit_probe