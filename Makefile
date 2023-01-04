VSTFLAGS = -Llibs/SimplePluginHost/export/lib -l:libSimplePluginHost.a
ifeq ($(OS),Windows_NT)
VSTFLAGS = -Llibs/SimplePluginHost/Builds/VisualStudio2022/x64/Release/Static\ Library -l:SimplePluginHost.lib
endif
VSTINCLUDE = -I libs/SimplePluginHost/export/include
VSTHOBJ = $(wildcard libs/SimplePluginHost/SimplePluginHost/Builds/LinuxMakefile/build/intermediate/*.o)
ifeq ($(OS),Windows_NT)
VSTHOBJ = 
endif
VSTLIB = libs/SimplePluginHost/export/lib/libSimplePluginHost.a
ifeq ($(NO_VST), 1)
VSTLIB =
VSTFLAGS =
VSTINCLUDE =
endif

IDIRS = $(addprefix -I ,$(shell find include -type d | sed -z 's/\n/ /g'))\
			-I libs/libremidi/include/ -I libs/stbi -I libs/obj\
			$(VSTINCLUDE)

ifeq ($(OS),Windows_NT)
IDIRS += -I ./mingw-std-threads
endif

CFLAGS = -std=c++17 -O3 -D NDEBUG
ifdef DEBUG
CFLAGS = -std=c++17 -O3
endif

LDFLAGS = $(IDIRS) -lglfw -lvulkan -ldl -lpthread -lasound -lopenal -lalut -lX11 -lXrandr -lcurl -lfreetype
ifeq ($(OS),Windows_NT)
LDFLAGS = $(IDIRS) -lglfw3 -lvulkan-1 -lopenal -lalut -lwinmm -lfreetype
endif

OBJDIR = bin/obj
LIBSOBJ = bin/libs/stbi/stb_image.o bin/libs/obj/tiny_obj_loader.o

SRCFOLDER = src
SRC = $(shell find $(SRCFOLDER) -type f -name '*.cpp' | sed -z 's/\n/ /g')
ifeq ($(NO_VST), 1)
SRC = $(shell find $(SRCFOLDER) -type f -name '*.cpp' -not -name 'AudioPluginHandler.cpp'\
			| sed -z 's/\n/ /g')
endif
OBJ = $(SRC:$(SRCFOLDER)/%.cpp=$(OBJDIR)/%.o)

# Testing files
MIDISRC = $(wildcard src/midi/*.cpp)
AUDIOSRC = $(wildcard src/audio/*.cpp)
JSONSRC = $(wildcard src/json/*.cpp)
VSTSRC = $(wildcard src/audio/*.cpp)
GSRC = $(wildcard src/render/*.cpp) $(wildcard src/render/*/*.cpp)\
				$(wildcard src/entity/*.cpp) $(wildcard src/gui/*.cpp)\
				$(wildcard src/anim/*.cpp) $(wildcard src/json/*.cpp)\
				$(wildcard src/audio/*.cpp) $(wildcard src/menu/*.cpp)\
				$(wildcard src/synthmania/*.cpp) $(wildcard src/midi/*.cpp)
MIDIOBJ = $(MIDISRC:$(SRCFOLDER)/%.cpp=$(OBJDIR)/%.o)
AUDIOOBJ = $(AUDIOSRC:$(SRCFOLDER)/%.cpp=$(OBJDIR)/%.o)
JSONOBJ = $(JSONSRC:$(SRCFOLDER)/%.cpp=$(OBJDIR)/%.o)
VSTOBJ = $(VSTSRC:$(SRCFOLDER)/%.cpp=$(OBJDIR)/%.o)
GOBJ = $(GSRC:$(SRCFOLDER)/%.cpp=$(OBJDIR)/%.o)

MODULEDIR = module

SHADERS = $(wildcard shader/*)

SHADERS_SPV = $(patsubst shader/%.comp, bin/%.comp.spv,\
				$(patsubst shader/%.vert, bin/%.vert.spv,\
				$(patsubst shader/%.frag, bin/%.frag.spv,\
				$(patsubst shader/%.geom, bin/%.geom.spv, $(SHADERS)))))

Synthmania: shader $(LIBSOBJ) $(VSTLIB) bin/config.json
	@make $(OBJ)
ifeq ($(OS),Windows_NT)
	g++ $(CFLAGS) -o bin/Synthmania $(OBJ) $(VSTHOBJ) $(LIBSOBJ) $(VSTLIB) $(DEBUG) $(LDFLAGS) $(VSTFLAGS)
else
	g++ $(CFLAGS) -rdynamic -o bin/Synthmania $(OBJ) $(VSTHOBJ) $(LIBSOBJ) $(VSTLIB) $(DEBUG) $(LDFLAGS) $(VSTFLAGS)
endif

bin/libs/stbi/stb_image.o: libs/stbi/stb_image.c
	@mkdir -p 'bin/libs/stbi'
	g++ $(CFLAGS) -c $< -o $@ -D STBI_NO_BMP -D STBI_NO_PSD -D STBI_NO_TGA -D STBI_NO_GIF -D STBI_NO_HDR -D STBI_NO_PIC -D STBI_NO_PNM

bin/libs/obj/tiny_obj_loader.o: libs/obj/tiny_obj_loader.cpp
	@mkdir -p 'bin/libs/obj/'
	g++ $(CFLAGS) -c $< -o $@

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
ifeq ($(OS),Windows_NT)
# We shall see if this works on windows
	g++ $(CFLAGS) -shared -Wl,--out-implib,lib$(MOD).a\
			$(shell find $(MODULEDIR)/$(MOD) -name '*.cpp') -I $(MODULEDIR)/$(MOD) \
			$(LDFLAGS) $(VSTFLAGS) $(DEBUG) -o bin/$(MOD).dll
else
	g++ $(CFLAGS) -fPIC -shared -rdynamic\
			$(shell find $(MODULEDIR)/$(MOD) -name '*.cpp') -I $(MODULEDIR)/$(MOD) \
			$(LDFLAGS) $(VSTFLAGS) $(DEBUG) -o bin/$(MOD).so
endif
endif

$(MODULEDIR)/$(MOD):
	$(error No module folder found ! Put your source code in $(MODULEDIR)/$(MOD))

bin/config.json:
	cp resources/default.json bin/config.json

test: Synthmania
	./bin/Synthmania

midi: $(MIDIOBJ)
	g++ $(CFLAGS) -o bin/MidiTest test/miditest.cpp\
		$(MIDIOBJ) $(LDFLAGS) $(DEBUG)

audio: $(AUDIOOBJ)
	g++ $(CFLAGS) -o bin/AudioTest  test/audiotest.cpp\
		$(AUDIOOBJ) $(LDFLAGS) $(DEBUG)

json: $(JSONOBJ)
	g++ $(CFLAGS) -o bin/JsonTest test/jsontest.cpp\
		$(JSONOBJ) $(LDFLAGS) $(DEBUG)

graphics: $(GOBJ) $(LIBSOBJ) shader
	g++ $(CFLAGS) -o bin/GraphicsTest test/graphicstest.cpp\
		$(GOBJ) $(LIBSOBJ) $(LDFLAGS) $(DEBUG) -D NOVST

vst: $(VSTOBJ) $(VSTLIB)
	g++ $(CFLAGS) -o bin/VstTest test/vsttest.cpp\
		$(VSTOBJ) $(VSTHOBJ) $(VSTLIB) $(VSTFLAGS) $(LDFLAGS) $(DEBUG)

bin/%.spv: shader/%
	glslc $< -o $@

shader:
	make $(SHADERS_SPV)

$(VSTLIB):
ifndef NO_VST
	make -C libs/SimplePluginHost
endif

clean:
	rm -rf bin/*
