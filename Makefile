VSTFLAGS = -L../SimplePluginHost/export/lib -l:libSimplePluginHost.a
ifeq ($(OS),Windows_NT)
VSTFLAGS = -L../SimplePluginHost/Builds/VisualStudio2022/x64/Release/Static\ Library -l:SimplePluginHost.lib
endif
VSTINCLUDE = -I ../SimplePluginHost/export/include
VSTOBJ = $(wildcard ../SimplePluginHost/Builds/LinuxMakefile/build/intermediate/*.o)
ifeq ($(OS),Windows_NT)
VSTOBJ = 
endif
VSTLIB = ../SimplePluginHost/export/lib/libSimplePluginHost.a
ifeq ($(NO_VST), 1)
VSTLIB =
VSTFLAGS =
VSTINCLUDE =
endif

IDIRS = $(addprefix -I ,$(shell find include -type d | sed -z 's/\n/ /g'))\
			-I libremidi/include/ -I stb -I obj $(VSTINCLUDE)

ifeq ($(OS),Windows_NT)
IDIRS += -I ./mingw-std-threads
endif

CFLAGS = -std=c++17 -O3 $(VSTOBJ) -D NDEBUG
LDFLAGS = $(IDIRS) -lglfw -lvulkan -ldl -lpthread -lasound -lopenal -lalut -lX11 -lXrandr -lcurl -lfreetype -lboost_filesystem
ifeq ($(OS),Windows_NT)
LDFLAGS = $(IDIRS) -lglfw3 -lvulkan-1 -lopenal -lalut -lwinmm -lfreetype
endif

OBJDIR = bin/obj
LIBS = obj/tiny_obj_loader.h stb/stb_image.h

SRCFOLDER = src
SRC = $(shell find $(SRCFOLDER) -type f -name '*.cpp' | sed -z 's/\n/ /g')
ifeq ($(NO_VST), 1)
SRC = $(shell find $(SRCFOLDER) -type f -name '*.cpp' -not -name 'AudioPluginHandler.cpp'\
			| sed -z 's/\n/ /g')
endif
OBJ = $(SRC:$(SRCFOLDER)/%.cpp=$(OBJDIR)/%.o)

# Testing files
MIDISRC = $(wildcard src/midi/*.cpp) test/miditest.cpp
AUDIOSRC = $(wildcard src/audio/*.cpp) test/audiotest.cpp
JSONSRC = $(wildcard src/json/*.cpp) test/jsontest.cpp
VSTSRC = $(wildcard src/audio/*.cpp) test/vsttest.cpp
GSRC = test/graphicstest.cpp $(wildcard src/render/*.cpp) $(wildcard src/render/*/*.cpp) \
				$(wildcard src/entity/*.cpp) $(wildcard src/gui/*.cpp) \
				$(wildcard src/anim/*.cpp) $(wildcard src/json/*.cpp) src/synthmania/Game.cpp

MODULEDIR = module

SHADERS = $(wildcard shader/*)

SHADERS_SPV = $(patsubst shader/%.vert, bin/%.vert.spv,\
				$(patsubst shader/%.frag, bin/%.frag.spv,\
				$(patsubst shader/%.geom, bin/%.geom.spv, $(SHADERS))))

Synthmania: shader $(VSTLIB)
	make $(OBJ)
ifeq ($(OS),Windows_NT)
	g++ $(CFLAGS) -o bin/Synthmania $(OBJ) $(VSTLIB) $(DEBUG) $(LDFLAGS) $(VSTFLAGS)
else
	g++ $(CFLAGS) -rdynamic -o bin/Synthmania $(OBJ) $(VSTLIB) $(DEBUG) $(LDFLAGS) $(VSTFLAGS)
endif

$(OBJDIR)/%.o: $(SRCFOLDER)/%.cpp
	@mkdir -p '$(@D)'
ifeq ($(NO_VST), 1)
	g++ $(CFLAGS) -D NOVST -c $< $(LDFLAGS) $(DEBUG) -o $@
else
	g++ $(CFLAGS) -c $< $(LDFLAGS) $(DEBUG) -o $@
endif

.PHONY: test clean shader obj module

module: $(MODULEDIR)/$(MOD)
ifndef MOD
	$(error Try again with the mod name : make module MOD=<modname>)
else
	g++ $(CFLAGS) -fPIC -shared -rdynamic $(shell find $(MODULEDIR)/$(MOD) -name '*.cpp') -I $(MODULEDIR)/$(MOD) \
			$(LDFLAGS) $(VSTFLAGS) $(DEBUG) -o bin/$(MOD).so
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

bin/%.spv: shader/%
	glslc $< -o $@

shader:
	make $(SHADERS_SPV)

$(VSTLIB):
ifndef NO_VST
	make -C ../SimplePluginHost
endif

clean:
	rm -rf bin/*
