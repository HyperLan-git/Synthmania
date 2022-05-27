#include <iostream>

#include "Chart.hpp"
#include "JsonHandler.hpp"

int main() {
    Chart c = readChart("resources/songs/ETEA/sdata.json");
    std::cout << "chart : \ntitle : " << c.name << "\nartist : " << c.artist
              << "\naudio : " << c.audio << "\noffset : " << c.offset
              << "\nanimation : " << c.animation << "\ndiffs :\n";
    for (auto &d : c.diffs) {
        std::cout << "name : " << d.name << "\nmidi : " << d.midi
                  << "\ntrack : " << d.track_id << std::endl;
    }
    return 0;
}