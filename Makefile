all: test

include make/*.mk

cpy_dlls:
ifeq ($(OS),Windows_NT)
	ldd bin/Synthmania.exe | grep "=> /" | grep -vi "c/windows" | awk '{print $$3}' | xargs -I{} cp "{}" -n bin/
	cp /mingw64/bin/vulkan-1.dll -n bin/
endif

package: Synthmania cpy_dlls
ifeq ($(OS),Windows_NT)
	zip -r game.zip bin/*.dll bin/Synthmania.exe bin/config.json resources game.bat $(SHADERS_SPV)
else
	zip -r game.zip bin/Synthmania.exe bin/config.json resources game.bat $(SHADERS_SPV)
endif

dependencies: detect-requirements check-env check-vulkan
	@echo -n "Are you sure? [y/N] " && read ans && if [ $${ans:-'N'} = 'y' ]; then make deps; fi

Synthmania: shader $(LIBSOBJ) $(VSTLIB) bin/config.json
	@make $(OBJ) -j 10
ifeq ($(OS),Windows_NT)
	g++ $(CFLAGS) -o bin/Synthmania $(OBJ) $(VSTHOBJ) $(LIBSOBJ) $(VSTLIB) $(DEBUG) $(LDFLAGS) $(VSTFLAGS)
else
	g++ $(CFLAGS) -rdynamic -o bin/Synthmania $(OBJ) $(VSTHOBJ) $(LIBSOBJ) $(VSTLIB) $(DEBUG) $(LDFLAGS) $(VSTFLAGS)
endif

binaries: $(LIBSOBJ) $(VSTLIB) $(OBJ)

test: Synthmania
	./bin/Synthmania

shader:
	make $(SHADERS_SPV)

clean:
	rm -rf bin/*

.PHONY: test clean shader obj module