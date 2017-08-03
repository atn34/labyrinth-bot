a.out: display.cc
	g++ -std=c++11 display.cc -I/usr/local/include/opencv `pkg-config --libs opencv`

.PHONY: clean
clean:
	rm a.out

.PHONY: run
run:
	LD_LIBRARY_PATH=/usr/local/lib ./a.out
