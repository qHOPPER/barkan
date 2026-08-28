CC = clang
CFLAGS = -O2 -g -D__x86_64__ -target bpf

all: src/xdp_barkan_v3.o src/sockops_barkan.o

src/xdp_barkan_v3.o: src/xdp_barkan_v3.c
	$(CC) $(CFLAGS) -c $< -o $@

src/sockops_barkan.o: src/sockops_barkan.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o
