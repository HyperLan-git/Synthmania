#include "MidiHandler.hpp"

int main(int argc, char** argv) {
    const char* path = "resources/songs/examplesong/bruh.MID";
    if (argc > 1) path = argv[1];
    MidiHandler handler;
    auto result = handler.readMidi(path);
    for (MidiNote note : result.notes) {
        std::cout << "Note " << (int)(note.note)
                  << "\ntime : " << (double)(note.timestamp) / 1000000.
                  << std::endl;
    }
    while (handler.getOpenPort() < 0) {
        if (handler.hasMessage()) {
            const Message& m = handler.getMessage();
            if (m.type != libremidi::message_type::NOTE_ON) continue;
            std::cout << "channel : " << m.channel << " note : " << m.data1
                      << std::endl;
        }
    }
    return 0;
}