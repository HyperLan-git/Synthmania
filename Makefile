IDIRS=$(addprefix -I ,$(wildcard include/**/) $(wildcard include/render/**/)) -I stb -I obj -I libremidi/include/libremidi

CFLAGS = -std=c++17 -O2
LDFLAGS = $(IDIRS) -lglfw -lvulkan -ldl -lpthread -lX11 -lXrandr

SRC=$(wildcard src/*.cpp) $(wildcard src/**/*.cpp) $(wildcard src/render/**/*.cpp)

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
