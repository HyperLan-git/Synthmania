#pragma once

class TextHandler;

#include <ft2build.h>

// Needs to be after the ft2build include
#include <freetype/freetype.h>
#include <freetype/ftbitmap.h>

#include "Device.hpp"
#include "Font.hpp"
#include "Image.hpp"
#include "ImageView.hpp"
#include "Utils.hpp"

class TextHandler {
   public:
    TextHandler(VkPhysicalDevice* physicalDevice, Device& device,
                unsigned int textureSize);

    TextHandler(TextHandler&&) = delete;
    TextHandler& operator=(TextHandler&&) = delete;

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

    std::vector<Font> getFonts();
    std::vector<TexPtr> getTextures();

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
    FT_Library lib = NULL;

    std::vector<Font> fonts;
    std::vector<TexPtr> textures;
    Device& device;

    unsigned int textureSize;
};

std::vector<std::shared_ptr<Gui>> printString(std::string text,
                                              TextHandler* textHandler,
                                              std::string entityNames,
                                              std::string font, double size,
                                              glm::vec2 pos,
                                              glm::vec4 color = {0, 0, 0, 1});

std::vector<std::shared_ptr<Gui>> printShadowedString(
    std::string text, TextHandler* textHandler, std::string entityNames,
    std::string font, double size, glm::vec2 pos,
    glm::vec4 color = {0, 0, 0, 1});

std::vector<std::shared_ptr<Gui>> printShakingString(
    std::string text, TextHandler* textHandler, std::string entityNames,
    std::string font, double size, glm::vec2 pos, float shake,
    glm::vec4 color = {0, 0, 0, 1});