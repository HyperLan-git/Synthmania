msys_version = 0
ifeq ($(OS), Windows_NT)
GPU = $(strip $(shell wmic path win32_VideoController get name | sed -n 2p))
msys_version = $(if $(findstring Msys, $(shell uname -o)),$(word 1, $(subst ., ,$(shell uname -r))),0)
else
GPU = $(strip $(shell lspci | grep ' VGA ' | cut -d" " -f 5- \
			| sed -E -e 's/(\[[^][]*\])|(\([^()]*\))//g' | sed -E -e 's/(Intel)|(Corporation)//g'))
endif

detect-requirements:
	$(info Checking GPU...)
ifeq (,$(GPU))
	$(error No GPU found on your system !!!)
endif
	@if install/check_gpu.sh $(GPU); then echo "Could not guarantee your GPU ($(GPU)) can handle vulkan."; fi

ifneq ($(OS),Windows_NT)
DISTRO = $(shell lsb_release -si)
VERSION = $(shell lsb_release -sr)
endif

check-env:
	$(info Checking environment...)
ifeq ($(OS),Windows_NT)
	$(info OS : $(OS))
ifeq ("pacman:","$(shell whereis pacman)")
	$(error pacman is not installed !!!)
endif
else
	$(info OS : $(DISTRO) $(VERSION))
ifneq ($(DISTRO),Ubuntu)
	$(error Deps for your OS not implemented !)
endif
ifeq ("apt:","$(shell whereis apt)")
	$(error apt-get is not installed !!!)
endif
endif
	$(info Environment checked...)

check-vulkan:
	$(info Testing vulkan installation...)
	@vkvia>/dev/null || make install-vulkan

install-vulkan:
	$(info Could not find vulkan, attempt to install...)
ifeq ($(OS),Windows_NT)
	$(info Installing from msys2 package vulkan sdk...)
	pacman -S mingw-w64-x86_64-vulkan-devel
else
ifeq ($(DISTRO),Ubuntu)
ifneq (,$(filter 18.04,20.04,22.04,$(VERSION)))
	$(info Installing from Ubuntu package vulkan sdk...)
	sudo apt-get install vulkan_sdk
else
	make vulkan-tarball
endif
else
	make vulkan-tarball
endif
endif
	$(info Vulkan installation complete, testing...)
	@vkvia

vulkan-tarball:
	$(info Attempting to install Vulkan from tarball...)
	@install/install-vulkan.sh
	@vkvia
	

deps:
ifeq ($(OS),Windows_NT)
	pacman -S mingw-w64-x86_64-boost mingw-w64-x86_64-glm mingw-w64-x86_64-openal mingw-w64-x86_64-freetype
else
	sudo apt-get update ; sudo apt-get upgrade ; sudo apt-get install g++ libglfw3-dev libglm-dev libopenal-dev libfreetype-dev libboost-all-dev libasound2-dev
endif