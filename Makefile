VSTHEADERS=../SimplePluginHost/export/include
VSTOBJ=$(wildcard ../SimplePluginHost/Builds/LinuxMakefile/build/intermediate/*.o)
VSTLIB=../SimplePluginHost/export/lib/libSimplePluginHost.a
IDIRS=$(addprefix -I ,$(wildcard include/**/) $(wildcard include/render/**/)) -I libremidi/include/ -I stb -I obj -I $(VSTHEADERS)

CFLAGS = -std=c++17 -O2 $(VSTOBJ)
LDFLAGS = $(IDIRS) -lglfw -lvulkan -ldl -lpthread -lasound -lopenal -lalut -lX11 -lXrandr -lcurl -lfreetype \
				-L../SimplePluginHost/export/lib -l:libSimplePluginHost.a -g

OBJDIR=bin/obj
LIBS=obj/tiny_obj_loader.h stb/stb_image.h

SRC=$(wildcard src/*.cpp) $(wildcard src/**/*.cpp) $(wildcard src/render/**/*.cpp)
OBJ=$(SRC:.cpp=.o)

# Testing files
MIDISRC=$(wildcard src/midi/*.cpp) test/miditest.cpp
AUDIOSRC=$(wildcard src/audio/*.cpp) test/audiotest.cpp
JSONSRC=$(wildcard src/json/*.cpp) test/jsontest.cpp
VSTSRC=test/vsttest.cpp

Synthmania: shader $(VSTLIB)
	d=$$(date +%s) ; \
	make $(OBJ) ; \
	g++ $(CFLAGS) -o bin/Synthmania $(OBJ) $(VSTLIB) $(LDFLAGS) \
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

vst: $(VSTLIB)
	g++ $(CFLAGS) -o bin/VstTest $(VSTSRC) $(VSTOBJ) $(VSTLIB) $(LDFLAGS)

shader:
	glslc shader/gui.vert -o bin/vert_gui.spv
	glslc shader/def.vert -o bin/vert.spv
	glslc shader/def.frag -o bin/frag.spv

$(VSTLIB):
	make -C ../SimplePluginHost

clean:
	rm -f $(OBJ)
	rm -rf bin/*
