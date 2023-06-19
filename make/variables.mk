# audio plugins related
ifeq ($(VST), 1)
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
endif

IDIRS = $(shell pkg-config --cflags freetype2)\
		$(addprefix -I ,$(shell find include -type d | sed -z 's/\n/ /g'))\
			-I libs/stbi -I libs/obj -I libs/libremidi/include/\
			$(VSTINCLUDE)

ifeq ($(OS),Windows_NT)
IDIRS += -I ./mingw-std-threads
endif

CFLAGS = -std=c++17 -O3
ifdef DEBUG
CFLAGS = -std=c++17 -O0 -g -D DEBUG
endif

LDFLAGS = $(IDIRS) -lglfw -lvulkan -ldl -lpthread -lasound -lopenal -lX11 -lXrandr -lcurl -lfreetype
ifeq ($(OS),Windows_NT)
LDFLAGS = $(IDIRS) -lglfw3 -lvulkan-1 -lopenal -lwinmm -lfreetype
endif

OBJDIR = bin/obj
LIBSOBJ = bin/libs/stbi/stb_image.o bin/libs/obj/tiny_obj_loader.o

SRCFOLDER = src
ifeq ($(VST), 1)
SRC = $(shell find $(SRCFOLDER) -type f -name '*.cpp' | sed -z 's/\n/ /g')
else
SRC = $(shell find $(SRCFOLDER) -type f -name '*.cpp' -not -name 'AudioPluginHandler.cpp'\
			| sed -z 's/\n/ /g')
endif
OBJ = $(SRC:$(SRCFOLDER)/%.cpp=$(OBJDIR)/%.o)

MODULEDIR = module

SHADERS = $(wildcard shader/*)

SHADERS_SPV = $(patsubst shader/%.comp, bin/%.comp.spv,\
				$(patsubst shader/%.vert, bin/%.vert.spv,\
				$(patsubst shader/%.frag, bin/%.frag.spv,\
				$(patsubst shader/%.geom, bin/%.geom.spv, $(SHADERS)))))
