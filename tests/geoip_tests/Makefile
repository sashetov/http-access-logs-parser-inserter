DEBUG=-g
CFLAGS=-O2 -Wall -W -fno-strict-aliasing -fno-omit-frame-pointer
export PKG_CONFIG_PATH='/usr/local/lib/pkgconfig'
GEOIP_CFLAGS=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags geoip)
GEOIP_LDFLAGS=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs geoip)

all: clean benchmark
benchmark: benchmark.o
	$(CC) -o benchmark benchmark.o $(GEOIP_LDFLAGS)
benchmark.o: benchmark
	$(CC) -c benchmark.c $(CFLAGS) $(GEOIP_CFLAGS) $(DEBUG)
clean:
	rm -rf *.o benchmark
