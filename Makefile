CC=clang++
LD=clang++
CFLAGS=--std=c++11 -O3
#CFLAGS=--std=c++11 -O3 -DDEBUG
LDFLAGS=-lmpfr

HEADERS=$(wildcard *.hpp)

.PHONY: all
all: p t

.PHONY: clean
clean:
	rm p t
	rm *.o

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

t: parsedbl.o test.o
	$(LD) $(LDFLAGS) -o $@ $+

p: parsedbl.o p_main.o
	$(LD) $(LDFLAGS) -o $@ $+
