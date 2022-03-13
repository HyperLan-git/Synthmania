CFLAGS = -std=c++17 -O2
LDFLAGS = -idirafter ../stb -lglfw -lvulkan -ldl -lpthread -lX11 -lXrandr

SRC=$(wildcard *.cpp)

compile: shader
	g++ $(CFLAGS) -o VulkanTest $(SRC) $(LDFLAGS)

.PHONY: test clean shader

test: compile
	./VulkanTest

debug: shader
	g++ $(CFLAGS) -o VulkanTest $(SRC) $(LDFLAGS) -fsanitize=address -static-libasan -g

shader:
	glslc ../shader/def.vert -o vert.spv
	glslc ../shader/def.frag -o frag.spv

clean:
	rm -f vert.spv frag.spv VulkanTest
