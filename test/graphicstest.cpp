#include <ft2build.h>
#define FT
#include <freetype/freetype.h>

int main() {
    FT_Library* lib = new FT_Library();
    FT_Init_FreeType(lib);

    FT_Done_FreeType(*lib);
    delete lib;
    return 0;
}