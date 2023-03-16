# Synthmania
A midi rhythm game. Simple as that.

Plop in any midi file and test how funny a song sounds when you miss every other note or play percussions in the wrong places.
Oh and I guess if you're an actual musician you can learn songs with this piece of software.
Going as far as possible with what modern rhythm gaming can do is my goal while always keeping musicality at the center... Whatever that means.

# Installation
You need the right drivers installed on your system with a suitable graphics card that supports vulkan.

I mainly tested my environment on an ubuntu system (manually installing vulkan),
but I also work on a windows environment (with msys2 mingw) to make sure my code works on both (a lot of things don't rn).

Try using `make dependencies` to install everything needed.

# Compilation
Simply use the makefile.
Use DEBUG="compilation-options" to add more compilation options like -g.

Commands to compile for the first time (should install needed deps if needed) :
```sh
git submodule update --init --recursive
make dependencies
make
```

## Modules
This part is not final and will probably have to be replaced with a more portable scripting language (I hate those when they are badly designed), but right now shader modifications as well as animations in charts work with dynamic cpp libraries.
Simply add in a folder in /module your source code (cpp and hpp files) and use `make module MOD="folder"`.
Go crazy and don't be scared to destroy everything.

## Dependencies
You will need to have :
- Vulkan SDK with glslc `https://vulkan.lunarg.com/sdk/home`
- GLFW `sudo apt-get install libglfw3-dev` `https://www.glfw.org/`
- glm `sudo apt-get install libglm-dev` `https://github.com/g-truc/glm`
- OpenAL `sudo apt-get install libopenal-dev` `https://www.openal.org/downloads/`
- freetype `sudo apt-get install libfreetype-dev` `https://freetype.org/download.html`
- boost `sudo apt-get install libboost-all-dev` `https://www.boost.org/users/download/`
- JUCE (to work on the plugin host) `https://juce.com/download/`

Imma keep the latest versions of those and make sure project compiles with them (hopefully).
Shit happens. lol

Windows build needs a shitton of dlls right now (I should fix that in the compilation process...). Idk how I'm going to handle that for the end user. Probably a crappy windows installer hehe

ALUT is a crutch and makes you weak.

# Development
Here is what the game looks like for people who don't want to/know how to compile and try the thing for themself (I'll post some demos and more info when more ppl get interested) :
- [youtube](https://www.youtube.com/channel/UCkYoZyuOyPuBcerhdJB67Zw)
- [discord](https://discord.gg/WnU84Aqtnb)

Maybe I'll actually do something right and code short scripts to detect your platform and install everything automatically (WIP but try).