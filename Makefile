all: test

include make/*.mk

dependencies: detect-requirements check-env check-vulkan
	@echo -n "Are you sure? [y/N] " && read ans && if [ $${ans:-'N'} = 'y' ]; then make deps; fi

Synthmania: shader $(LIBSOBJ) $(VSTLIB) bin/config.json
	@make $(OBJ)
ifeq ($(OS),Windows_NT)
	g++ $(CFLAGS) -o bin/Synthmania $(OBJ) $(VSTHOBJ) $(LIBSOBJ) $(VSTLIB) $(DEBUG) $(LDFLAGS) $(VSTFLAGS)
else
	g++ $(CFLAGS) -rdynamic -o bin/Synthmania $(OBJ) $(VSTHOBJ) $(LIBSOBJ) $(VSTLIB) $(DEBUG) $(LDFLAGS) $(VSTFLAGS)
endif

.PHONY: test clean shader obj module

test: Synthmania
	./bin/Synthmania

shader:
	make $(SHADERS_SPV)

clean:
	rm -rf bin/*
