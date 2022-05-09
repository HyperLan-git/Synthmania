#include "JsonHandler.hpp"

JsonHandler::JsonHandler() {}

tree* JsonHandler::read(const char* json) {
    tree* t = new tree();
    boost::property_tree::json_parser::read_json(json, *t);
    return t;
}