#pragma once
#include "Chart.hpp"

class JsonHandler;

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/property_tree/json_parser.hpp>
#include <map>

#include "Chart.hpp"

typedef boost::property_tree::ptree tree;

tree* readJson(const std::string json);
Chart readChart(const std::string json);

void readTree(tree &t, std::map<std::string, std::string>& result,
              std::string path);
