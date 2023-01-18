CFLAGS = -std=c++17 -O2

RasterizerTest: *.cpp
	g++ $(CFLAGS) -o rasteRize *.cpp

.PHONY: test clean

test: rayTest
	./rasteRize

clean:
	rm -f rasteRize
