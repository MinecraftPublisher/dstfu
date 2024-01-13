clean:
	rm -rf dstfu .DS_Store

build: clean
	clang dstfu.c -o dstfu -framework CoreFoundation -framework CoreServices

run: build
	sudo ./dstfu

install: build
	cp ./dstfu /usr/local/bin/dstfu