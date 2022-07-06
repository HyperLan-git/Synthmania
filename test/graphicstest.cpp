#include <ft2build.h>
#define FT
#include <freetype/freetype.h>

#include "Renderer.hpp"

#define FONT "./resources/fonts/Stupid.ttf"
#define FONT "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"

FT_Face getFont(const std::string& fontname, int pt, FT_Library* lib) {
    FT_Face face;
    if (FT_New_Face(*lib, fontname.c_str(), 0, &face)) return NULL;
    if (FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(pt))) return NULL;
    return face;
}

FT_UInt getGlyph(uint16_t c, FT_Face face) {
    return FT_Get_Char_Index(face, c);
}

class Test : public Game {
   public:
    Test(int width, int height, const char* title, FT_Library* lib)
        : Game(width, height, title) {
        Renderer* r = renderer;
        VkPhysicalDevice* pd = r->getPhysicalDevice();
        Device* device = r->getDevice();
        FT_Face face = getFont(FONT, 32, lib);
        FT_GlyphSlot glyphSlot = face->glyph;
        FT_UInt i = getGlyph('a', face);
        FT_Load_Glyph(face, i, FT_LOAD_DEFAULT);
        FT_Render_Glyph(glyphSlot, FT_RENDER_MODE_NORMAL);
        uint w = glyphSlot->bitmap.width, h = glyphSlot->bitmap.rows;
        std::vector<uint8_t> buffer(w * h * 4);

        uint8_t* src = glyphSlot->bitmap.buffer;
        uint8_t* startOfLine = src;
        int dst = 0;

        for (int y = 0; y < h; ++y) {
            src = startOfLine;
            for (int x = 0; x < w; ++x) {
                auto value = *src;
                src++;

                buffer[dst++] = 0xff;
                buffer[dst++] = 0xff;
                buffer[dst++] = 0xff;
                buffer[dst++] = value;
            }
            startOfLine += glyphSlot->bitmap.pitch;
        }
        VkDeviceSize imageSize = w * h * 4;

        Image* image = new Image(
            pd, device, w, h, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        r->transitionImageLayout(image, VK_IMAGE_LAYOUT_UNDEFINED,
                                 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        void* d;
        vkMapMemory(*(device->getDevice()), *(image->getMemory()->getMemory()),
                    0, buffer.size(), 0, &d);
        memcpy(d, buffer.data(), (size_t)buffer.size());
        vkUnmapMemory(*(device->getDevice()),
                      *(image->getMemory()->getMemory()));
        r->addTexture(image, "text");
    }

    void init() {
        addGui(
            new Gui(getTextureByName(renderer->getTextures(), "text"), "text"));
    }

    void update() {}

    ~Test() {}

   private:
};

int main() {
    FT_Library* lib = new FT_Library();
    FT_Init_FreeType(lib);
    Test* game = new Test(1920, 1080, "GraphicsTest", lib);
    Renderer* r = game->getRenderer();

    game->run();
    delete game;
    FT_Done_FreeType(*lib);
    delete lib;
    return 0;
}