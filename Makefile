a.out: display.cc
	g++ -std=c++11 display.cc `pkg-config --cflags --libs opencv`

.PHONY: clean
clean:
	rm a.out

.PHONY: run
run: a.out
	LD_LIBRARY_PATH=/usr/local/lib ./a.out
