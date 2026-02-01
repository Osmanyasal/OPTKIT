CC ?= gcc
CXX ?= g++

# Adjust if you build a different configuration.
OPTKIT_LIB_DIR ?= ../../bin/Release

CFLAGS += -I../../src -O2 -Wall -Wextra
LDFLAGS += -L$(OPTKIT_LIB_DIR)
LDLIBS += -loptkit_c -loptkit_dynamic -lm -lpthread

EXAMPLES = c_api_demo perf query_cpu query_gpu energy_cpu disk temperature

all: $(EXAMPLES)

c_api_demo: main.c ../../src/optkit_c.h
	$(CC) $(CFLAGS) -o $@ main.c $(LDFLAGS) $(LDLIBS)

perf: perf.c ../../src/optkit_c.h
	$(CC) $(CFLAGS) -o $@ perf.c $(LDFLAGS) $(LDLIBS)

query_cpu: query_cpu.c ../../src/optkit_c.h
	$(CC) $(CFLAGS) -o $@ query_cpu.c $(LDFLAGS) $(LDLIBS)

query_gpu: query_gpu.c ../../src/optkit_c.h
	$(CC) $(CFLAGS) -o $@ query_gpu.c $(LDFLAGS) $(LDLIBS)

energy_cpu: energy_cpu.c ../../src/optkit_c.h
	$(CC) $(CFLAGS) -o $@ energy_cpu.c $(LDFLAGS) $(LDLIBS)

disk: disk.c ../../src/optkit_c.h
	$(CC) $(CFLAGS) -o $@ disk.c $(LDFLAGS) $(LDLIBS)

temperature: temperature.c ../../src/optkit_c.h
	$(CC) $(CFLAGS) -o $@ temperature.c $(LDFLAGS) $(LDLIBS)

run: c_api_demo
	LD_LIBRARY_PATH="$(OPTKIT_LIB_DIR):$$LD_LIBRARY_PATH" ./c_api_demo

clean:
	rm -f $(EXAMPLES)
