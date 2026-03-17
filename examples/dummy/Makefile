CC ?= gcc
CXX ?= g++

# Adjust if you build a different configuration.
OPTKIT_LIB_DIR ?= ../../bin/Release

CFLAGS += -I../../src -I../../lib/spdlog/include -O2 -Wall -Wextra
LDFLAGS += -L$(OPTKIT_LIB_DIR)
LDLIBS += -loptkit_dynamic -lm -lpthread

DUMMY = main

all: $(DUMMY)

main: main.cc
	$(CXX) $(CFLAGS) -o $@ main.cc $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(DUMMY)
