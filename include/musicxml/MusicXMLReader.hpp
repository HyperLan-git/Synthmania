#pragma once

class MusicXMLReader;

#include <boost/property_tree/xml_parser.hpp>
#include <vector>

class MusicXMLReader {
   public:
    MusicXMLReader(std::string xmlData);

   private:
    // std::vector<MusicPart> parts;
    // std::vector<Credit> credits;
    // Metadata identification;
};