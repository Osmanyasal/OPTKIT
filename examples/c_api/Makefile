CC ?= gcc
CXX ?= g++

# Adjust if you build a different configuration.
OPTKIT_LIB_DIR ?= ../../bin/Release

CFLAGS += -I../../src -O2 -Wall -Wextra
LDFLAGS += -L$(OPTKIT_LIB_DIR)
LDLIBS += -loptkit_c -loptkit_dynamic -lm -lpthread

all: c_api_demo
 
c_api_demo: main.c ../../src/optkit_c.h
	$(CC) $(CFLAGS) -o $@ main.c $(LDFLAGS) $(LDLIBS)

run: c_api_demo
	LD_LIBRARY_PATH="$(OPTKIT_LIB_DIR):$$LD_LIBRARY_PATH" ./c_api_demo

clean:
	rm -f c_api_demo
