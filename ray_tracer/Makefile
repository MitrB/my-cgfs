CFLAGS = -std=c++17 -O2

VulkanTest: main.cpp
	g++ $(CFLAGS) -o rayTest main.cpp

.PHONY: test clean

test: rayTest
	./rayTest

clean:
	rm -f rayTest
