#include "JsonHandler.hpp"

tree* read(const char* json) {
    tree* t = new tree();
    boost::property_tree::json_parser::read_json(json, *t);
    return t;
}

Chart readChart(const char* json) {
    Chart c;
    tree* t = read(json);
    std::string str = "None";
    uint64_t n = 0;
    uint i = 0;
    c.animation = t->get<std::string>("animation", str);
    c.artist = t->get<std::string>("artist", str);
    c.audio = t->get<std::string>("audio", str);
    c.name = t->get<std::string>("name", str);
    c.offset = t->get<uint64_t>("offset", n);
    tree diffs = t->get_child("diffs");
    for (auto elem : diffs) {
        tree diff = elem.second;
        Diff d;
        d.name = diff.get<std::string>("name", str);
        d.track_id = diff.get<uint>("track_id", i);
        d.midi = diff.get<std::string>("midi", str);
        c.diffs.push_back(d);
    }
    delete t;
    return c;
}