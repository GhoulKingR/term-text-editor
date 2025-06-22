
build:
	mkdir build
	gcc main.c -o build/main

run: build
	./build/main

clean:
	rm -r build
