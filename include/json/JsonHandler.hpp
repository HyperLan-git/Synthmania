#pragma once
#include "Chart.hpp"

class JsonHandler;

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/property_tree/json_parser.hpp>
#include <map>

#include "Chart.hpp"

typedef boost::property_tree::ptree tree;

tree* readJson(const char* json);
Chart readChart(const char* json);

void readTree(tree t, std::map<std::string, std::string>& result,
              std::string path);
