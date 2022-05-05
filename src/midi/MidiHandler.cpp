#include "MidiHandler.hpp"

const char *notes[] = {"Do",  "Do#", "Re",   "Re#", "Mi",  "Fa",
                       "Fa#", "Sol", "Sol#", "La",  "La#", "Si"};

size_t std::hash<libremidi::message>::operator()(
    libremidi::message const &message) const {
    return ((message.bytes[0] << 7) | (message.bytes[1] << 6) |
            (message.bytes[2] << 5) | (message.bytes[2] << 4)) ^
           (int)(666556507 * message.timestamp);
}

MidiHandler::MidiHandler() {
    start_time = micros();
    /*libremidi::midi_out out;
    for (int i = 0, N = out.get_port_count(); i < N; i++) {
        // Information on port number i
        std::string name = out.get_port_name(i);
        std::cout << "out:" << name << std::endl;
    }
    libremidi::midi_in midi;
    for (int i = 0, N = midi.get_port_count(); i < N; i++) {
        // Information on port number i
        std::string name = midi.get_port_name(i);
        std::cout << "in:" << name << std::endl;
    }
    midi.open_port(0);

    // Setup a callback for incoming messages.
    // This should be done immediately after opening the port
    // to avoid having incoming messages written to the
    // queue instead of sent to the callback function.

    // Note that the callback will be invoked from a separate thread,
    // it is up to you to protect your data structures afterwards.
    // For instance if you are using a GUI toolkit, don't do GUI actions
    // in that callback !
    midi.set_callback([this](const libremidi::message &message) {
        // int bit = message[0] >> 7; // Should always be 1
        int type = (message[0] >> 4) - 8;
        int channel = (message[0]) & 0xF;
        Message m;
        if (message.is_note_on_or_off()) {
            int note = message[1];
            int velocity = message[2];
            const char *n = notes[note % 12];
            int octave = note / 12 - 1;
            m.type = message.get_message_type();
            m.channel = channel;
            m.data1 = message[1];
            m.data2 = (message.bytes.size() > 2) ? message[2] : 0;
            m.timestamp = micros() - this->start_time;
            this->messages.push(m);
            // std::cout << "note pressed : " << n << octave << std::endl;
        } else if (message[0] <
                   uint8_t(libremidi::message_type::SYSTEM_EXCLUSIVE)) {
            m.type = message.get_message_type();
            m.channel = -1;
            m.data1 = message[1];
            m.data2 = (message.bytes.size() > 2) ? message[2] : 0;
            m.timestamp = micros() - this->start_time;
            this->messages.push(m);
        }
    });*/
}

TrackPartition MidiHandler::readMidi(const char *path) {
    std::ifstream file(path, std::ios::binary);

    std::vector<uint8_t> bytes;
    bytes.assign(std::istreambuf_iterator<char>(file),
                 std::istreambuf_iterator<char>());

    // Initialize our reader object
    libremidi::reader r;

    // Parse
    libremidi::reader::parse_result result = r.parse(bytes);
    std::vector<MidiNote> notes;

    uint64_t MPQ = 125000;
    // If parsing succeeded, use the parsed data
    if (result == libremidi::reader::invalid) return TrackPartition{MPQ, notes};
    // Pitch wheel : 0x2000 = 8192 = +-0 semitones 0x0 = -2 semitones
    // and 0x3FFF = +2 semitones
    std::vector<MidiNote> currentNotes;
    for (auto &track : r.tracks) {
        uint64_t t = 0;
        for (auto &event : track) {
            libremidi::message message = event.m;
            uint64_t dt = event.tick * 1000000 / r.ticksPerBeat / 4 * 120 /
                          r.startingTempo;
            switch (message.get_message_type()) {
                case libremidi::message_type::NOTE_ON:
                    for (auto iter = currentNotes.begin();
                         iter != currentNotes.end(); iter++) {
                        if ((*iter).note == message.bytes[1] &&
                            (t >= (*iter).timestamp)) {
                            uint64_t length = t - (*iter).timestamp;
                            (*iter).length = length;
                            notes.push_back(*iter);
                            currentNotes.erase(iter);
                            break;
                        }
                    }
                    if (message[2] != 0) {
                        currentNotes.push_back(
                            {t, 0, message.bytes[1],
                             message.bytes[2]});  // velocity == 0 => note off
                    }
                    break;
                case libremidi::message_type::NOTE_OFF:
                    for (auto iter = currentNotes.begin();
                         iter != currentNotes.end(); iter++) {
                        if ((*iter).note == message.bytes[1] &&
                            (t > (*iter).timestamp)) {
                            uint64_t length = t - (*iter).timestamp;
                            (*iter).length = length;
                            notes.push_back(*iter);
                            currentNotes.erase(iter);
                            break;
                        }
                    }
                    break;
                case libremidi::message_type::PITCH_BEND:
                    break;
                case libremidi::message_type::CONTROL_CHANGE:
                    break;
                case libremidi::message_type::PROGRAM_CHANGE:
                    break;
                case libremidi::message_type::SYSTEM_RESET:
                    break;
                default:
                    std::cout << "msg=" << (int)message.get_message_type()
                              << std::endl;
                    break;
            }
            t += dt;
        }
        currentNotes.clear();
    }

    file.close();

    return TrackPartition{MPQ, notes};
}

bool MidiHandler::hasMessage() { return !messages.empty(); }

Message MidiHandler::getMessage() {
    if (messages.empty())
        return {libremidi::message_type::INVALID, -1, 0, 0, 0};
    Message val;
    messages.pop(val);
    return val;
}

MidiHandler::~MidiHandler() {}

uint64_t micros() {
    uint64_t us =
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
            .count();
    return us;
}