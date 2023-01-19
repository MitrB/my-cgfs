CFLAGS = -std=c++17 -O3

VulkanTest: main.cpp
	g++ $(CFLAGS) -o rayTest main.cpp

.PHONY: test clean

test: rayTest
	./rayTest | feh out.ppm

clean:
	rm -f rayTest
