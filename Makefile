default: build/simple_a build/simple_so build/multiple_a build/multiple_so

CC     	 ?= clang
CFLAGS 	 ?= -Wall -Wextra -pedantic -I./include/ -g

RAYLIB_DIR ?= ./include/raylib-5.5_linux_amd64
RAYLIB_A = -I$(RAYLIB_DIR)/include -L$(RAYLIB_DIR)/lib -l:libraylib.a -lm 
RAYLIB_SO= -I$(RAYLIB_DIR)/include -L$(RAYLIB_DIR)/lib -lraylib -lm 

build/simple_a: examples/simple.c include/graph.h src/graph.c lib/graph.a 
	mkdir -p build
	$(CC) $(CFLAGS) -O0 -xc $< -o $@ $(RAYLIB_A) -L./lib -l:graph.a

build/simple_so: examples/simple.c include/graph.h src/graph.c lib/libgraph.so
	mkdir -p build
	$(CC) $(CFLAGS) -O0 -xc $< -o $@ $(RAYLIB_SO) -L./lib -lgraph

build/multiple_a: examples/multiple.c include/graph.h src/graph.c lib/graph.a 
	mkdir -p build
	$(CC) $(CFLAGS) -O0 -xc $< -o $@ $(RAYLIB_A) -L./lib -l:graph.a

build/multiple_so: examples/multiple.c include/graph.h src/graph.c lib/libgraph.so
	mkdir -p build
	$(CC) $(CFLAGS) -O0 -xc $< -o $@ $(RAYLIB_SO) -L./lib -lgraph

build/graph.o: include/graph.h src/graph.c 
	mkdir -p build
	$(CC) -c $(CFLAGS) -xc src/graph.c -o $@ $(RAYLIB_A) 

lib/graph.a: build/graph.o include/graph.h src/graph.c 
	mkdir -p lib
	ar rcv $@ $<

lib/libgraph.so: include/graph.h src/graph.c 
	mkdir -p lib
	$(CC) -shared $(CFLAGS) -xc src/graph.c -o $@ $(RAYLIB_SO) -lm

.PHONY: clean
clean:
	rm -fr build
	rm -fr lib

