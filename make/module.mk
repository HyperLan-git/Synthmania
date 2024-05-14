
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
