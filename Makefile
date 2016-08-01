
all: out/Default/lib.target/libunio.so

re: clean all

clean:
	@rm -rf ./out/Default/obj.target/unio out/Default/lib.target/libunio.so

out/Default/lib.target/libunio.so: build/gyp
	@./gyp_unio.py -f make unio.gyp
	@make -C out CFLAGS=-fPIC

build/gyp:
	@git clone https://chromium.googlesource.com/external/gyp.git build/gyp

.PHONY: clean
