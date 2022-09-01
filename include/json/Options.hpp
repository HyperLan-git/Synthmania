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

    template <typename T>
    std::optional<T> getValue(std::string path);

    template <typename T>
    void setValue(std::string path, T value);

    std::string save();

    virtual ~Options();

   private:
    tree defaultValues;
    tree options;
};