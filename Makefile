.PHONY: doxygen
doxygen:
	doxygen

.PHONY: clean
clean:
	rm -rf a.out html latex

.PHONY: run
run: 
	./build.py | make a.out -f -
	./a.out

.PHONY: test
test:
	./build.py | make test.out -f -
	./test.out
