optkit_rapl_read: rapl_read.cc
	g++ rapl_read.cc -o optkit_rapl_read

run: optkit_rapl_read
	while true; do ./optkit_rapl_read -p; done

clean:
	rm ./optkit_rapl_read
