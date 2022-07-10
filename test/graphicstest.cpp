#include <ft2build.h>
#define FT
#include <freetype/freetype.h>
#include <freetype/ftbitmap.h>

#include "Renderer.hpp"

#define FONT "./resources/fonts/Stupid.ttf"

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
        FT_Face face = getFont(FONT, 128, lib);
        FT_GlyphSlot glyphSlot = face->glyph;
        FT_UInt i = getGlyph('H', face);
        FT_Load_Glyph(face, i, FT_LOAD_DEFAULT);
        FT_Render_Glyph(glyphSlot, FT_RENDER_MODE_NORMAL);
        unsigned int w = glyphSlot->bitmap.width + 2,
                     h = glyphSlot->bitmap.rows + 2;
        uint8_t buffer[w * h * 4] = {0};
        for (int i = 0; i < w * h * 4; i++) buffer[i] = 255;

        uint8_t* src = glyphSlot->bitmap.buffer;
        for (int y = 0; y < h - 2; y++) {
            for (int x = 0; x < w - 2; x++) {
                std::cout << ((((int)src[x + y * (w - 2)]) > 120) ? 'O' : ' ')
                          << " ";
            }
            std::cout << '\n';
        }

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                if (y == 0 || y == h - 1 || x == 0 || x == w - 1) {
                    buffer[(x + y * w) * 4 + 3] = 0;
                    continue;
                }
                uint8_t value = src[(x - 1) + (y - 1) * (w - 2)];
                buffer[(x + y * w) * 4 + 3] = value;
            }
        }

        Image* image = new Image(
            pd, device, w, h, VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_TILING_LINEAR,  // Fricking tiling
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        r->transitionImageLayout(image, VK_IMAGE_LAYOUT_UNDEFINED,
                                 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        void* d;
        vkMapMemory(*(device->getDevice()), *(image->getMemory()->getMemory()),
                    0, w * h * 4, 0, &d);
        memcpy(d, buffer, w * h * 4);
        vkUnmapMemory(*(device->getDevice()),
                      *(image->getMemory()->getMemory()));
        r->addTexture(image, "text");
    }

    void init() {
        Gui* g = new Gui(getTextureByName(renderer->getTextures(), "missing"),
                         "back");
        g->setSize({5.f, 5.f});
        addGui(g);
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