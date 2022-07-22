VSTFLAGS = -L../SimplePluginHost/export/lib -l:libSimplePluginHost.a
ifeq ($(OS),Windows_NT)
VSTFLAGS = -L../SimplePluginHost/Builds/VisualStudio2019/x64/Release/Static\ Library -l:SimplePluginHost.lib
endif
VSTHEADERS = ../SimplePluginHost/export/include
VSTOBJ = $(wildcard ../SimplePluginHost/Builds/LinuxMakefile/build/intermediate/*.o)
VSTLIB = ../SimplePluginHost/export/lib/libSimplePluginHost.a
ifeq ($(OS),Windows_NT)
VSTLIB = ../SimplePluginHost/export/lib/SimplePluginHost.lib
endif
IDIRS = $(addprefix -I ,$(shell find include -type d | sed -z 's/\n/ /g'))\
			-I libremidi/include/ -I stb -I obj -I $(VSTHEADERS) \
			-I /usr/include/freetype2/

ifeq ($(OS),Windows_NT)
IDIRS += -I mingw-std-threads
endif

CFLAGS = -std=c++17 -O3 $(VSTOBJ)
LDFLAGS = $(IDIRS) -lglfw -lvulkan -ldl -lpthread -lasound -lopenal -lalut -lX11 -lXrandr -lcurl -lfreetype
ifeq ($(OS),Windows_NT)
LDFLAGS = $(IDIRS) -lglfw3 -lvulkan-1 -lopenal -lalut -lwinmm -lfreetype
endif

OBJDIR = bin/obj
LIBS = obj/tiny_obj_loader.h stb/stb_image.h

SRCFOLDER = src
SRC = $(shell find $(SRCFOLDER) -type f -name '*.cpp' | sed -z 's/\n/ /g')
OBJ = $(SRC:$(SRCFOLDER)/%.cpp=$(OBJDIR)/%.o)

# Testing files
MIDISRC = $(wildcard src/midi/*.cpp) test/miditest.cpp
AUDIOSRC = $(wildcard src/audio/*.cpp) test/audiotest.cpp
JSONSRC = $(wildcard src/json/*.cpp) test/jsontest.cpp
VSTSRC = $(wildcard src/audio/*.cpp) test/vsttest.cpp
GSRC = test/graphicstest.cpp $(wildcard src/render/*.cpp) $(wildcard src/render/*/*.cpp) \
				$(wildcard src/entity/*.cpp) $(wildcard src/gui/*.cpp) $(wildcard src/json/*.cpp) src/synthmania/Game.cpp

MODULEDIR = module

Synthmania: shader $(VSTLIB)
	make $(OBJ)
	g++ $(CFLAGS) -rdynamic -o bin/Synthmania $(OBJ) $(VSTLIB) $(DEBUG) $(LDFLAGS) $(VSTFLAGS)

$(OBJDIR)/%.o: $(SRCFOLDER)/%.cpp
	@mkdir -p '$(@D)'
	g++ $(CFLAGS) -c $< $(LDFLAGS) $(DEBUG) -o $@

.PHONY: test clean shader obj module

module: $(MODULEDIR)/$(MOD)
ifndef MOD
	$(error Try again with the mod name : make module MOD=<modname>)
else
	g++ $(CFLAGS) -fPIC -shared -rdynamic $(shell find $(MODULEDIR) -name '*.cpp') -I $(MODULEDIR) \
			$(LDFLAGS) $(VSTFLAGS) -o bin/$(MOD).so
endif

$(MODULEDIR)/$(MOD):
	$(error No module folder found ! Put your source code in $(MODULEDIR)/$(MOD))

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
	g++ $(CFLAGS) -o bin/VstTest $(VSTSRC) $(VSTOBJ) $(VSTLIB) $(VSTFLAGS) $(LDFLAGS) -DNDEBUG

shader:
	glslc shader/gui.vert -o bin/vert_gui.spv
	glslc shader/def.vert -o bin/vert.spv
	glslc shader/def.geom -o bin/geom.spv
	glslc shader/def.frag -o bin/frag.spv

$(VSTLIB):
	make -C ../SimplePluginHost

clean:
	rm -rf bin/*
