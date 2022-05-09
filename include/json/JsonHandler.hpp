#pragma once

class JsonHandler;

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/property_tree/json_parser.hpp>

typedef boost::property_tree::ptree tree;

class JsonHandler {
   public:
    JsonHandler();
    tree* read(const char* json);
};