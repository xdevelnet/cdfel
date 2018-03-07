all:
	true
shared:
	rm -Rf build/
	mkdir build
	cc --std=c99 -fPIC -Wall -Werror -O2 -c src/cdfel.c -o build/cdfel.o
	cc -shared build/cdfel.o -o build/cdfel.so
clean:
	rm -Rf build/
