#include "TextHandler.hpp"

TextHandler::TextHandler(VkPhysicalDevice* physicalDevice, Device* device,
                         unsigned int textureSize) {
    this->physicalDevice = physicalDevice;
    this->device = device;
    this->textureSize = textureSize;
    lib = new FT_Library();
    if (FT_Init_FreeType(lib))
        throw std::runtime_error("Could not init freetype !");
}

Image* TextHandler::loadCharacter(FT_Face face, unsigned long character,
                                  CommandPool* commandPool) {
    FT_GlyphSlot glyphSlot = face->glyph;
    FT_UInt i = FT_Get_Char_Index(face, character);
    FT_Load_Glyph(face, i, FT_LOAD_DEFAULT);
    FT_Render_Glyph(glyphSlot, FT_RENDER_MODE_NORMAL);
    unsigned int w = glyphSlot->bitmap.width + 2,
                 h = glyphSlot->bitmap.rows + 2;

    uint8_t* bitmap = glyphSlot->bitmap.buffer;

    Image* image =
        new Image(physicalDevice, device, w, h, VK_FORMAT_R8G8B8A8_SRGB,
                  VK_IMAGE_TILING_LINEAR,  // Fricking tiling
                  VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT |
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // TODO put all that in image class
    VkImageSubresource sub;
    sub.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    sub.mipLevel = 0;
    sub.arrayLayer = 0;
    VkSubresourceLayout layout;
    vkGetImageSubresourceLayout(*(device->getDevice()), *(image->getImage()),
                                &sub, &layout);
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

    CommandBuffer* commandBuffer = new CommandBuffer(device, commandPool, true);
    commandBuffer->begin();

    commandBuffer->setImageLayout(image, VK_IMAGE_LAYOUT_UNDEFINED,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    commandBuffer->end();
    commandBuffer->submit(device->getQueue("secondary"));
    delete commandBuffer;
    void* d;
    // TODO utilitary function to copy data to memory
    vkMapMemory(*(device->getDevice()), *(image->getMemory()->getMemory()), 0,
                layout.rowPitch * h, 0, &d);
    memcpy(d, buffer, layout.rowPitch * h);
    vkUnmapMemory(*(device->getDevice()), *(image->getMemory()->getMemory()));
    // TODO copy image to other image with tiling optimal and less memory usage
    return image;
}

void TextHandler::loadFonts(
    std::map<std::string, std::vector<unsigned long>> fontsToLoad,
    CommandPool* commandPool) {
    for (auto entry : fontsToLoad) {
        FT_Face f;
        if (FT_New_Face(*lib, entry.first.c_str(), 0, &f)) continue;
        if (FT_Set_Pixel_Sizes(f, 0, textureSize)) continue;

        Font font;
        font.name = f->family_name;

        for (unsigned long c : entry.second) {
            std::string charName = f->family_name;
            charName.append("_");
            charName.append(std::to_string(c));
            addTexture(loadCharacter(f, c, commandPool), charName.c_str());
            Character chr;
            chr.texture = getTextureByName(textures, charName.c_str());
            chr.character = c;
            chr.width = f->glyph->bitmap.width + 2;
            chr.height = f->glyph->bitmap.rows + 2;
            chr.advance = f->glyph->linearHoriAdvance / 65536;
            chr.vAdvance = f->glyph->linearVertAdvance / 65536;
            chr.offsetTop = f->glyph->bitmap_top;
            chr.offsetLeft = f->glyph->bitmap_left;
            font.characters.emplace(c, chr);
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

// TODO make an atlas :/
void TextHandler::addTexture(Image* texture, const char* name) {
    ImageView* view = new ImageView(device, texture, VK_FORMAT_R8G8B8A8_SRGB,
                                    VK_IMAGE_ASPECT_COLOR_BIT, name);
    this->textures.push_back(view);
}

std::vector<Text> TextHandler::createText(std::string text,
                                          std::string fontName, double size,
                                          glm::vec2 start) {
    std::vector<Text> result;
    double conv = 64. * textureSize / size;
    for (int i = 0; i < text.size(); i++) {
        Character c = getCharacter(fontName, (unsigned long)text[i]);
        Text t;
        t.character = c;
        t.pos = {
            start.x + (double)c.width / conv / 2 - c.offsetLeft / conv * 0.7,
            start.y + (double)c.height / conv / 2 - c.offsetTop / conv * 0.7};
        t.size = {(double)c.width / conv, (double)c.height / conv};
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
        t.pos = {
            start.x + (double)c.width / conv / 2 - c.offsetLeft / conv * 0.7,
            start.y + (double)c.height / conv / 2 - c.offsetTop / conv * 0.7};
        t.size = {(double)c.width / conv, (double)c.height / conv};
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
        t.pos = {
            start.x + (double)c.width / conv / 2 - c.offsetLeft / conv * 0.7,
            start.y + (double)c.height / conv / 2 - c.offsetTop / conv * 0.7};
        t.size = {(double)c.width / conv, (double)c.height / conv};
        result.push_back(t);
        start.y += (c.vAdvance) / conv * .7;
    }
    return result;
}

TextHandler::~TextHandler() {
    FT_Done_FreeType(*lib);
    delete lib;
}