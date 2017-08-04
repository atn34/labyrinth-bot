a.out: display.cc
	g++ -std=c++11 display.cc `pkg-config --cflags --libs opencv`

.PHONY: doxygen
doxygen:
	doxygen

.PHONY: clean
clean:
	rm -rf a.out html latex

.PHONY: run
run: a.out
	LD_LIBRARY_PATH=/usr/local/lib ./a.out
