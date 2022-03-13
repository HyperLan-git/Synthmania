CFLAGS = -std=c++17 -O2
LDFLAGS = -idirafter include -idirafter stb -lglfw -lvulkan -ldl -lpthread -lX11 -lXrandr

SRC=$(wildcard src/*.cpp)

VulkanTest: shader
	g++ $(CFLAGS) -o bin/VulkanTest $(SRC) $(LDFLAGS)

.PHONY: test clean shader

test: VulkanTest
	./bin/VulkanTest
shader:
	glslc shader/def.vert -o bin/vert.spv
	glslc shader/def.frag -o bin/frag.spv

clean:
	rm -f bin/*
