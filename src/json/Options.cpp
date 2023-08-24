#include "Options.hpp"

Options::Options(std::string defaults, std::string current) {
    this->path = current;
    tree *def = readJson(defaults), *cur = readJson(current);
    boost::optional<int> opt = cur->get_optional<int>("audio.device");

    defaultValues = def;
    options = cur;
}

std::optional<tree> Options::getValues(std::string path) {
    auto result = this->options->get_child_optional(path);
    if (result) return std::make_optional(result.get());
    result = this->defaultValues->get_child_optional(path);
    return result ? std::make_optional(*result) : std::optional<tree>();
}

void Options::setValues(std::string path, tree arr) {
    this->options->put_child(path, arr);
}

std::string Options::getPath() { return path; }

std::string Options::save() {
    std::ostringstream oss;
    boost::property_tree::json_parser::write_json(oss, *options);
    return oss.str();
}

Options::~Options() {
    delete defaultValues;
    delete options;
}