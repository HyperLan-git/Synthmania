
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
		$(GOBJ) $(LIBSOBJ) $(LDFLAGS) $(DEBUG)

vst: $(VSTOBJ) $(VSTLIB)
	g++ $(CFLAGS) -o bin/VstTest test/vsttest.cpp\
		$(VSTOBJ) $(VSTHOBJ) $(VSTLIB) $(VSTFLAGS) $(LDFLAGS) $(DEBUG) -D VST
