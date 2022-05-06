#include <iostream>

#include "AudioHandler.hpp"

int main() {
    std::vector<std::string> devices = getDevices();
    for (auto device : devices) std::cout << device << std::endl;
    AudioHandler* handler = new AudioHandler();
    while (handler->update())
        ;
    delete handler;
}