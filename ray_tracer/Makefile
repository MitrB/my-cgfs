CFLAGS = -std=c++17 -O3

VulkanTest: *.cpp
	g++ $(CFLAGS) -o rayTest *.cpp

.PHONY: test clean

test: rayTest
	./rayTest | feh out.ppm

clean:
	rm -f rayTest
