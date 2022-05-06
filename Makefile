IDIRS=$(addprefix -I ,$(wildcard include/**/) $(wildcard include/render/**/)) -I stb -I obj -I libremidi/include/

CFLAGS = -std=c++17 -O2
LDFLAGS = $(IDIRS) -lglfw -lvulkan -ldl -lpthread -lasound -lopenal -lalut -lX11 -lXrandr

OBJDIR=bin/obj
OBJ=libremidi/include/libremidi/*.cpp stb/stb_image.h

SRC=$(wildcard src/*.cpp) $(wildcard src/**/*.cpp) $(wildcard src/render/**/*.cpp)
MIDISRC=$(wildcard src/midi/*.cpp) test/miditest.cpp
AUDIOSRC=$(wildcard src/audio/*.cpp) test/audiotest.cpp

VulkanTest: shader
	g++ $(CFLAGS) -o bin/VulkanTest $(SRC) $(LDFLAGS)

.PHONY: test clean shader

test: VulkanTest
	./bin/VulkanTest

midi:
	g++ $(CFLAGS) -o bin/MidiTest $(MIDISRC) $(LDFLAGS)

audio:
	g++ $(CFLAGS) -o bin/AudioTest $(AUDIOSRC) $(LDFLAGS)

shader:
	glslc shader/gui.vert -o bin/vert_gui.spv
	glslc shader/def.vert -o bin/vert.spv
	glslc shader/def.frag -o bin/frag.spv

clean:
	rm -f bin/*
