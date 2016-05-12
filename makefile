ifeq ($(origin CC), default)
    CC:=clang
endif
export CC

override CFLAGS += -I/usr/local/include/
export CFLAGS

LIBS:=/usr/local/Cellar/openssl/1.0.2h/lib/libcrypto.a

DIRS:=$(shell ls -F -1 src | grep '/$$' | sed 's/\/$$//g' )
OBJS:=$(shell for dir in $(DIRS); do ls -1 src/$$dir | grep '\.c$$' | sed "s/\(.*\)\.c$$/build\/$$dir\/\1.o/g"; done )

default: dhtd makefile
.PHONY: default

-include $(patsubst)

dhtd: $(OBJS) src/main.c
	$(CC) $^ $(LIBS) -o dhtd

build/%.o: src/%.c | build
	$(MAKE) -f ../../dirMaker.mk -C $(dir $<) ../../$@

build:
	mkdir build

clean:
	-rm -r build
	-rm dhtd
	for dir in $(DIRS); do $(MAKE) -f ../../dirMaker.mk -C src/$$dir clean; done
