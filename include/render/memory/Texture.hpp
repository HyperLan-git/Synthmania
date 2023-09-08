#pragma once

class Texture;

#include <string>

/**
 * @brief A class to identify textures. That's right. It's just an identifier
 * to simplify and optimize interactions between game code and the engine. You
 * can copy this object all you want, modify, destroy it. It won't affect images
 * drawn. You'll have to get down and dirty with @ref{ImageView} ImageView
 * to actually affect anything.
 */
class Texture {
   public:
    Texture();

    explicit Texture(std::string id);

    bool operator==(const Texture& other) const;

    bool operator!=(const Texture& other) const;

    int64_t operator-(const Texture& other) const;

    bool operator<(const Texture& other) const;

    // Overload hash function for unordered_map
    struct HashFunction {
        std::size_t operator()(const Texture& id) const;
    };

   private:
    std::string identifier;
};