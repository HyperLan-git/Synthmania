include make/variables.mk

# Config files
bin/config.json:
	cp resources/default.json bin/config.json

# shaders
bin/%.spv: shader/%
	glslc $< -o $@

# libs
bin/libs/stbi/stb_image.o: libs/stbi/stb_image.c
	@mkdir -p 'bin/libs/stbi'
	g++ $(CFLAGS) -c $< -o $@ -D STBI_NO_BMP -D STBI_NO_PSD -D STBI_NO_TGA -D STBI_NO_GIF -D STBI_NO_HDR -D STBI_NO_PIC -D STBI_NO_PNM

bin/libs/obj/tiny_obj_loader.o: libs/obj/tiny_obj_loader.cpp
	@mkdir -p 'bin/libs/obj/'
	g++ $(CFLAGS) -c $< -o $@

ifdef VST
$(VSTLIB):
	make -C libs/SimplePluginHost
endif

# main compilation recipe
$(OBJDIR)/%.o: $(SRCFOLDER)/%.cpp $(INCLUDEFOLDER)/%.hpp
	@mkdir -p '$(@D)'
ifeq ($(VST), 1)
	g++ $(CFLAGS) -D VST -c $< $(LDFLAGS) $(DEBUG) -o $@
else
	g++ $(CFLAGS) -c $< $(LDFLAGS) $(DEBUG) -o $@
endif

# For files without header
$(OBJDIR)/%.o: $(SRCFOLDER)/%.cpp
	@mkdir -p '$(@D)'
ifeq ($(VST), 1)
	g++ $(CFLAGS) -D VST -c $< $(LDFLAGS) $(DEBUG) -o $@
else
	g++ $(CFLAGS) -c $< $(LDFLAGS) $(DEBUG) -o $@
endif