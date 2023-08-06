#include "TextHandler.hpp"

TextHandler::TextHandler(VkPhysicalDevice* physicalDevice, Device* device,
                         unsigned int textureSize) {
    this->physicalDevice = physicalDevice;
    this->device = device;
    this->textureSize = textureSize;
    if (FT_Init_FreeType(&lib))
        throw std::runtime_error("Could not init freetype !");
}

Image* TextHandler::loadCharacter(FT_Face face, unsigned long character,
                                  CommandPool* commandPool) {
    FT_GlyphSlot glyphSlot = face->glyph;
    FT_UInt i = FT_Get_Char_Index(face, character);
    FT_Load_Glyph(face, i, FT_LOAD_DEFAULT);
    FT_Render_Glyph(glyphSlot, FT_RENDER_MODE_NORMAL);
    const unsigned int w = glyphSlot->bitmap.width + 2,
                       h = glyphSlot->bitmap.rows + 2;

    uint8_t* bitmap = glyphSlot->bitmap.buffer;

    Image* image =
        new Image(physicalDevice, device, w, h, VK_FORMAT_R8G8B8A8_SRGB,
                  VK_IMAGE_TILING_LINEAR,  // Fricking tiling
                  VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT |
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    VkSubresourceLayout layout = image->getImageSubresourceLayout();
    uint8_t buffer[layout.rowPitch * h] = {0};
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            if (y == 0 || y == h - 1 || x == 0 || x == w - 1) {
                buffer[(x + y * layout.rowPitch / 4) * 4 + 3] = 0;
                continue;
            }
            uint8_t value =
                bitmap[(x - 1) + (y - 1) * (glyphSlot->bitmap.pitch)];
            buffer[(x + y * layout.rowPitch / 4) * 4 + 3] = value;
        }

    {
        CommandBuffer commandBuffer(device, commandPool, true);
        commandBuffer.begin();

        commandBuffer.setImageLayout(image, VK_IMAGE_LAYOUT_UNDEFINED,
                                     VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

        commandBuffer.end();
        commandBuffer.submit(device->getQueue("secondary"));
    }
    image->write(buffer, layout.rowPitch * h, 0);
    // TODO copy image to other image with tiling optimal and less memory usage
    return image;
}

void TextHandler::loadFonts(
    std::map<std::string, std::vector<unsigned long>> fontsToLoad,
    CommandPool* commandPool) {
    Queue* queue = device->getQueue("secondary");
    for (const auto& entry : fontsToLoad) {
        FT_Face f;
        if (FT_New_Face(lib, entry.first.c_str(), 0, &f)) continue;
        if (FT_Set_Pixel_Sizes(f, 0, textureSize)) continue;

        LayeredAtlas* atlas = new LayeredAtlas(
            this->device,
            new Image(
                this->physicalDevice, this->device, textureSize, textureSize,
                VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_TILING_LINEAR,  // XXX Fricking tiling
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT |
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                entry.second.size()),
            VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT,
            std::string("font_") + f->family_name);

        {
            CommandBuffer buf(device, commandPool, true);
            buf.begin();
            buf.setImageLayout(atlas->getImage(), VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0,
                               entry.second.size());
            buf.end();
            buf.submit(queue);
        }

        Font font;
        font.textures = atlas;
        font.name = f->family_name;
        uint32_t i = 0;
        for (const unsigned long c : entry.second) {
            std::string charName = f->family_name;
            charName.append("_");
            charName.append(std::to_string(c));
            Image* ch = loadCharacter(f, c, commandPool);
            {
                CommandBuffer buf(device, commandPool, true);
                buf.begin();
                buf.setImageLayout(ch, VK_IMAGE_LAYOUT_UNDEFINED,
                                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
                buf.end();
                buf.submit(queue);
            }

            Character chr;
            chr.texture = atlas->getTexture(atlas->append(ch, commandPool));
            this->textures.push_back(chr.texture);
            chr.character = c;
            chr.width = f->glyph->bitmap.width + 2;
            chr.height = f->glyph->bitmap.rows + 2;
            chr.advance = f->glyph->linearHoriAdvance / 65536;
            chr.vAdvance = f->glyph->linearVertAdvance / 65536;
            chr.offsetTop = f->glyph->bitmap_top;
            chr.offsetLeft = f->glyph->bitmap_left;
            font.characters.emplace(c, chr);
            delete ch;
            i++;
        }
        {
            CommandBuffer buf(device, commandPool, true);
            buf.begin();
            buf.setImageLayout(atlas->getImage(),
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0,
                               entry.second.size());
            buf.end();
            buf.submit(queue);
        }

        fonts.push_back(font);
    }
}

std::vector<Font> TextHandler::getFonts() { return fonts; }

std::vector<ImageView*> TextHandler::getTextures() { return textures; }

unsigned int TextHandler::getTextureSize() { return textureSize; }

Character TextHandler::getCharacter(std::string fontName, unsigned long code) {
    for (Font f : fonts)
        if (fontName.compare(f.name) == 0) return f.characters[code];

    return Character({0, 0, 0, 0, 0, 0, 0, NULL});
}

// TODO redo character drawing cause these coefficients fucked up man...
std::vector<Text> TextHandler::createText(std::string text,
                                          std::string fontName, double size,
                                          glm::vec2 start) {
    std::vector<Text> result;
    double conv = 64. * textureSize / size;
    for (int i = 0; i < text.size(); i++) {
        Character c = getCharacter(fontName, (unsigned long)text[i]);
        Text t;
        t.character = c;
        t.pos = {start.x + (double)textureSize / conv / 2 - c.offsetLeft / conv,
                 start.y + (double)textureSize / conv / 2 - c.offsetTop / conv};

        t.size = {(double)textureSize / conv, (double)textureSize / conv};
        result.push_back(t);
        start.x += (c.advance) / conv;
    }
    return result;
}

std::vector<Text> TextHandler::createText_w(std::wstring text,
                                            std::string fontName, double size,
                                            glm::vec2 start) {
    std::vector<Text> result;
    double conv = 64. * textureSize / size;
    for (int i = 0; i < text.size(); i++) {
        Character c = getCharacter(fontName, (unsigned long)text[i]);
        Text t;
        t.character = c;
        t.pos = {start.x + (double)textureSize / conv / 2 - c.offsetLeft / conv,
                 start.y + (double)textureSize / conv / 2 - c.offsetTop / conv};

        t.size = {(double)textureSize / conv, (double)textureSize / conv};
        result.push_back(t);
        start.x += (c.advance) / conv;
    }
    return result;
}

std::vector<Text> TextHandler::createVerticalText(std::string text,
                                                  std::string fontName,
                                                  double size,
                                                  glm::vec2 start) {
    std::vector<Text> result;
    double conv = 64. * this->textureSize / size;
    for (int i = 0; i < text.size(); i++) {
        Character c = getCharacter(fontName, (unsigned long)text[i]);
        Text t;
        t.character = c;
        t.pos = {start.x +
                     (double)(this->textureSize * 2 - c.width) / conv / 2 -
                     c.offsetLeft / conv * .75,
                 start.y + (double)this->textureSize / conv / 2 -
                     c.offsetTop / conv * .75};
        t.size = {(double)this->textureSize / conv,
                  (double)this->textureSize / conv};
        result.push_back(t);
        start.y += (c.vAdvance) / conv * .7;
    }
    return result;
}

TextHandler::~TextHandler() {
    // TODO for (Font& f : fonts) delete f.textures;
    FT_Done_FreeType(lib);
}