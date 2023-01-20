# Synthmania
An 88key rythm game. Simple as that.
I got motivation for making this project after recieving my very own synth and getting it to work with a midi cable.
Also have an e-drumset so lets get hittin'.

# Installation
You need Vulkan installed on your system with a suitable graphics card.

# Compilation
Simply use the makefile.
Use DEBUG="compilation-options" to add more compilation options like -g.

## Modules
This part is not final and will probably have to be replaced with a more portable scripting language (I hate those when they are badly designed), but right now shader modifications as well as animations in charts work with dynamic cpp libraries.
Simply add in a folder in /module your source code (cpp and hpp files) and use `make module MOD="folder"`.
Go crazy and don't be scared to destroy everything.

## Dependencies
You will need to add in your system :
- Vulkan and GLFW with glm
- OpenAL
- JUCE
- glslc
- boost

Imma keep the latest versions of those and make sure project compiles with them (hopefully).
I have no clue what I'm doing but I thought it would be a good idea to make headers organized in a file system without having to ask yourself where the header is when coding.
This means that you will have to -I every subfolder of include and the includes of every library used that is not immediately available to the compiler. You could also just put every header in the same folder and compile from there. lol

Windows build needs a shitton of dlls right now. Idk how I'm going to handle that for the end user. Probably a crappy windows installer hehe

ALUT is a crutch and makes you weak.

# Development
Here is what the game looks like for people who don't want to/know how to compile and try the thing for themself (I'll post some demos and more info when more ppl get interested) :
- [youtube](https://www.youtube.com/channel/UCkYoZyuOyPuBcerhdJB67Zw)
- [discord](https://discord.gg/WnU84Aqtnb)

Maybe I'll actually do something right and code short scripts to detect your platform and install everything automatically.

Nah that's too much work I'd rather remake Synthesia using 200h of spare time just to avoid paying 30€.