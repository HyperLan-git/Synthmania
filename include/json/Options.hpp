#pragma once

class Options;

#include <optional>
#include <string>
#include <vector>

#include "JsonHandler.hpp"
#include "Utils.hpp"

class Options {
   public:
    Options(std::string defaults, std::string current);

    // reminder to keep template definitions in the header file
    template <typename T>
    std::optional<T> getValue(std::string path) {
        boost::optional<T> result = this->options->get_optional<T>(path);
        if (result) return std::make_optional<T>(*result);
        result = this->defaultValues->get_optional<T>(path);
        return result ? std::make_optional<T>(*result) : std::optional<T>();
    }

    template <typename T>
    void setValue(std::string path, T value) {
        tree t;
        t.put_value<T>(value);
        this->options->put_child(path, t);
    }

    std::optional<tree> getValues(std::string path);
    void setValues(std::string path, tree arr);

    std::string save();

    std::string getPath();

    virtual ~Options();

   private:
    tree *defaultValues;
    tree *options;
    std::string path;
};