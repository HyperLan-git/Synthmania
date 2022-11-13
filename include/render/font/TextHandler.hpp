#pragma once

class TextHandler;

#include <ft2build.h>
#include <vulkan/vulkan.h>

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

// Needs to be after the ft2build include
#include <freetype/freetype.h>
#include <freetype/ftbitmap.h>

#include "Device.hpp"
#include "Font.hpp"
#include "Image.hpp"
#include "Utils.hpp"

class TextHandler {
   public:
    TextHandler(VkPhysicalDevice* physicalDevice, Device* device,
                unsigned int textureSize);

    TextHandler(const TextHandler&) = delete;
    TextHandler& operator=(const TextHandler&) = delete;

    Image* loadCharacter(FT_Face face, unsigned long character,
                         CommandPool* commandPool);

    /**
     * @brief Whenever you use this function you need to reload all textures
     * from the renderer
     *
     * @param fontsToLoad a map containing all characters to load
     * @param commandPool a command pool to use during the operations
     */
    void loadFonts(
        std::map<std::string, std::vector<unsigned long>> fontsToLoad,
        CommandPool* commandPool);

    void addTexture(Image* texture, const char* name);

    std::vector<Font> getFonts();
    std::vector<ImageView*> getTextures();

    unsigned int getTextureSize();

    Character getCharacter(std::string fontName, unsigned long code);

    std::vector<Text> createText_w(std::wstring text, std::string fontName,
                                   double size, glm::vec2 start);

    std::vector<Text> createText(std::string text, std::string fontName,
                                 double size, glm::vec2 start);

    std::vector<Text> createVerticalText(std::string text, std::string fontName,
                                         double size, glm::vec2 start);

    ~TextHandler();

   private:
    FT_Library* lib;

    std::vector<Font> fonts;
    std::vector<ImageView*> textures;
    VkPhysicalDevice* physicalDevice;
    Device* device;

    unsigned int textureSize;
};
