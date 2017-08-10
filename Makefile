.PHONY: build
build:
	./build.py | make a.out -f -

.PHONY: doxygen
doxygen:
	doxygen

.PHONY: clean
clean:
	rm -rf a.out html latex
	find . -type f -name '*.o' -delete
	find . -type f -name '*.exe' -delete

.PHONY: run
run: build
	./a.out

.PHONY: test
test:
	./build.py | make test -f -
