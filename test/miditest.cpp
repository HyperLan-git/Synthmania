#include "MidiHandler.hpp"
int main(int argc, char** argv) {
    const char* path = "resources/bruh.MID";
    if (argc > 1) path = argv[1];
    MidiHandler handler;
    auto result = handler.readMidi(path);
    for (MidiNote note : result) {
        std::cout << "Note " << (int)(note.note)
                  << "\ntime : " << (double)(note.timestamp) / 1000000.
                  << std::endl;
    }
    return 0;
}