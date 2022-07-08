VSTFLAGS = -L../SimplePluginHost/export/lib -l:libSimplePluginHost.a
VSTHEADERS = ../SimplePluginHost/export/include
VSTOBJ = $(wildcard ../SimplePluginHost/Builds/LinuxMakefile/build/intermediate/*.o)
VSTLIB = ../SimplePluginHost/export/lib/libSimplePluginHost.a
IDIRS = $(addprefix -I ,$(shell find include -type d | sed -z 's/\n/ /g'))\
			-I libremidi/include/ -I stb -I obj -I $(VSTHEADERS) \
			-I /usr/include/freetype2/

CFLAGS = -std=c++17 -O2 $(VSTOBJ)
LDFLAGS = $(IDIRS) -lglfw -lvulkan -ldl -lpthread -lasound -lopenal -lalut -lX11 -lXrandr -lcurl -lfreetype


OBJDIR = bin/obj
LIBS = obj/tiny_obj_loader.h stb/stb_image.h

SRC = $(shell find src -type f -name '*.cpp' | sed -z 's/\n/ /g')
OBJ = $(SRC:.cpp=.o)

# Testing files
MIDISRC = $(wildcard src/midi/*.cpp) test/miditest.cpp
AUDIOSRC = $(wildcard src/audio/*.cpp) test/audiotest.cpp
JSONSRC = $(wildcard src/json/*.cpp) test/jsontest.cpp
VSTSRC = $(wildcard src/audio/*.cpp) test/vsttest.cpp
GSRC = test/graphicstest.cpp $(wildcard src/render/*.cpp) $(wildcard src/render/*/*.cpp)\
				$(wildcard src/entity/*.cpp) $(wildcard src/gui/*.cpp) $(wildcard src/json/*.cpp) src/synthmania/Game.cpp

Synthmania: shader $(VSTLIB)
	d=$$(date +%s) ; \
	make $(OBJ) ; \
	g++ $(CFLAGS) -o bin/Synthmania $(OBJ) $(VSTLIB) $(LDFLAGS) $(VSTFLAGS) \
	&& echo "Build took $$(($$(date +%s)-d)) seconds"

%.o: %.cpp
	g++ $(CFLAGS) -c $< $(LDFLAGS) -o $@

.PHONY: test clean shader obj

test: Synthmania
	./bin/Synthmania

midi:
	g++ $(CFLAGS) -o bin/MidiTest $(MIDISRC) $(LDFLAGS)

audio:
	g++ $(CFLAGS) -o bin/AudioTest $(AUDIOSRC) $(LDFLAGS)

json:
	g++ $(CFLAGS) -o bin/JsonTest $(JSONSRC) $(LDFLAGS)

graphics:
	g++ $(CFLAGS) -o bin/GraphicsTest $(GSRC) $(LDFLAGS)

vst: $(VSTLIB)
	g++ $(CFLAGS) -o bin/VstTest $(VSTSRC) $(VSTOBJ) $(VSTLIB) $(VSTFLAGS) $(LDFLAGS) -fsanitize=address -DNDEBUG

shader:
	glslc shader/gui.vert -o bin/vert_gui.spv
	glslc shader/def.vert -o bin/vert.spv
	glslc shader/def.frag -o bin/frag.spv

$(VSTLIB):
	make -C ../SimplePluginHost

clean:
	rm -f $(OBJ)
	rm -rf bin/*
