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
    openPort(0);
}

std::vector<std::string> MidiHandler::getMidiPorts() {
    std::vector<std::string> result;
    for (int i = 0, N = in.get_port_count(); i < N; i++) {
        std::string name = in.get_port_name(i);
        result.push_back(name);
    }
    return result;
}

void MidiHandler::openPort(unsigned int port) {
    in.set_callback([this](const libremidi::message &message) {
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
    });
    in.open_port(port);
}

void MidiHandler::openPort(unsigned int port,
                           libremidi::midi_in::message_callback callback) {
    in.set_callback(callback);
    in.open_port(port);
}

TrackPartition MidiHandler::readMidi(const char *path) {
    std::ifstream file(path, std::ios::binary);

    std::vector<uint8_t> bytes;
    bytes.assign(std::istreambuf_iterator<char>(file),
                 std::istreambuf_iterator<char>());

    // Initialize our reader object
    libremidi::reader r = libremidi::reader(false);

    // Parse
    libremidi::reader::parse_result result = r.parse(bytes);
    std::vector<MidiNote> notes;

    r.startingTempo = 120;
    uint64_t MPQ = 250000 * 60 / r.startingTempo;

    // If parsing succeeded, use the parsed data
    if (result == libremidi::reader::invalid) return TrackPartition{MPQ, notes};
    // Pitch wheel : 0x2000 = 8192 = +-0 semitones 0x0 = -2 semitones
    // and 0x3FFF = +2 semitones
    std::vector<MidiNote> currentNotes;
    for (auto &track : r.tracks) {
        unsigned long long t = 0;
        for (auto &event : track) {
            libremidi::message message = event.m;
            unsigned long long dt = event.tick;
            dt = dt * 1000000 / r.ticksPerBeat * 60 / r.startingTempo;
            t += dt;
            switch (message.get_message_type()) {
                case libremidi::message_type::NOTE_ON:
                    for (auto iter = currentNotes.begin();
                         iter != currentNotes.end(); iter++) {
                        if ((*iter).note == message.bytes[1]) {
                            uint64_t length = t - (*iter).timestamp;
                            if (length == 0) {
                                std::cout << "f= " << message[0] << " "
                                          << message[1] << " " << message[2]
                                          << " " << message[3] << " "
                                          << message[4] << " " << message[5]
                                          << " ";
                            }
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
                        if ((*iter).note == message.bytes[1]) {
                            uint64_t length = t - (*iter).timestamp;
                            if (length == 0) {
                                std::cout << "f= " << message[0] << " "
                                          << message[1] << " " << message[2]
                                          << " " << message[3] << " "
                                          << message[4] << " " << message[5]
                                          << " ";
                            }
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
                    std::cout << "msg=" << std::hex << (int)message[0] << " "
                              << (int)message[1] << std::endl;
                    break;
                case libremidi::message_type::PROGRAM_CHANGE:
                    std::cout << "msg=" << std::hex << (int)message[0] << " "
                              << (int)message[1] << std::endl;
                    break;
                case libremidi::message_type::SYSTEM_RESET:
                    switch (message.get_meta_event_type()) {
                        case libremidi::meta_event_type::TEMPO_CHANGE:
                            MPQ = message[3];
                            MPQ = MPQ << 8;
                            MPQ += message[4];
                            MPQ = MPQ << 8;
                            MPQ += message[5];
                            r.startingTempo = (60 * 1000000.) / MPQ;
                            MPQ /= 4;  // TODO remove and put in demisemiquaver
                            break;
                        default:
                            std::cout << "msg=" << std::hex << (int)message[0]
                                      << " " << (int)message[1] << " "
                                      << (int)message[2] << std::endl;
                            break;
                    }
                    break;
                default:
                    std::cout << "msg=" << std::hex << (int)message[0] << " "
                              << (int)message[1] << std::endl;
                    break;
            }
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

MidiHandler::~MidiHandler() { in.set_callback(NULL); }

uint64_t micros() {
    uint64_t us =
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
            .count();
    return us;
}