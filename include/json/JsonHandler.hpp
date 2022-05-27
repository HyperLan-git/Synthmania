#pragma once

class JsonHandler;

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/property_tree/json_parser.hpp>

#include "Chart.hpp"

typedef boost::property_tree::ptree tree;

tree* read(const char* json);
Chart readChart(const char* json);