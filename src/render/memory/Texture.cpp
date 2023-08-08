#include "Texture.hpp"

Texture::Texture() : Texture("missing") {}

Texture::Texture(std::string id) : identifier(id) {}

bool Texture::operator==(const Texture& other) const {
    return other.identifier == this->identifier;
}

bool Texture::operator!=(const Texture& other) const {
    return !(other == *this);
}

int64_t Texture::operator-(const Texture& other) const {
    return Texture::HashFunction()(*this) - Texture::HashFunction()(other);
}

std::size_t Texture::HashFunction::operator()(const Texture& id) const {
    { return std::hash<std::string>{}(id.identifier); }
}