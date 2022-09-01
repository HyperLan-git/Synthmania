#include "Options.hpp"

Options::Options(std::string defaults, std::string current) {
    tree *def = readJson(defaults.c_str()), *cur = readJson(current.c_str());

    defaultValues = *def;
    options = *cur;

    delete def;
    delete cur;
}

template <typename T>
std::optional<T> Options::getValue(std::string path) {
    std::optional<T> result = this->options.get_optional<T>(path);
    if (result.has_value()) return result;
    return this->defaultValues.get_optional<T>(path);
}

template <typename T>
void Options::setValue(std::string path, T value) {
    tree t;
    t.put_value<T>(value);
    this->options.put_child(path, t);
}

std::string Options::save() {
    std::ostringstream oss;
    boost::property_tree::json_parser::write_json(oss, options);
    return oss.str();
}

Options::~Options() {}