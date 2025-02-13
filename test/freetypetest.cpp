/* example1.c                                                      */
/*                                                                 */
/* This small program shows how to print a rotated string with the */
/* FreeType 2 library.                                             */

// This is your reminder that if anything is broken with freetype you should
// compile it from source

#include <ft2build.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include FT_FREETYPE_H

#define WIDTH 150
#define HEIGHT 100

/* origin is the upper left corner */
unsigned char image[HEIGHT][WIDTH];

/* Replace this function with something useful. */

void draw_bitmap(FT_Bitmap* bitmap, FT_Int x, FT_Int y) {
    FT_Int i, j, p, q;
    FT_Int x_max = x + bitmap->width;
    FT_Int y_max = y + bitmap->rows;

    /* for simplicity, we assume that `bitmap->pixel_mode' */
    /* is `FT_PIXEL_MODE_GRAY' (i.e., not a bitmap font)   */

    if (!bitmap->buffer) throw "Character failed to render !";
    for (i = x, p = 0; i < x_max; i++, p++) {
        for (j = y, q = 0; j < y_max; j++, q++) {
            if (i < 0 || j < 0 || i >= WIDTH || j >= HEIGHT) continue;

            image[q][p] |= bitmap->buffer[q * bitmap->width + p];
        }
    }
}

void show_image(void) {
    int i, j;

    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++)
            putchar(image[i][j] == 0 ? '.' : image[i][j] < 128 ? '+' : '*');
        putchar('\n');
    }
}

const char* defaultargs[3] = {"", "resources/fonts/ARCADE_I.ttf", "AAa"};

int main(int argc, char** argv) {
    FT_Library library;
    FT_Face face;

    FT_GlyphSlot slot;
    FT_Matrix matrix; /* transformation matrix */
    FT_Vector pen;    /* untransformed origin  */
    FT_Error error;

    char* filename;
    char* text;

    double angle;
    int target_height;
    int n, num_chars;

    if (argc != 3) {
        argv = (char**)defaultargs;
    }
    filename = argv[1]; /* first argument     */
    text = argv[2];     /* second argument    */
    num_chars = strlen(text);
    angle = (25.0 / 360) * 3.14159 * 2;
    target_height = HEIGHT;

    std::cout << "Init ft" << std::endl;
    error = FT_Init_FreeType(&library); /* initialize library */
    if (error) {
        std::cerr << FT_Error_String(error) << std::endl;
        return 1;
    }

    std::cout << "Init face" << std::endl;
    error = FT_New_Face(library, filename, 0, &face); /* create face object */
    if (error) {
        std::cerr << FT_Error_String(error) << std::endl;
        return 1;
    }

    std::cout << "Loaded font: " << face->family_name
              << ", Number of faces: " << face->num_faces
              << ", Number of chars: " << face->num_glyphs << std::endl;

    std::cout << "Set char size" << std::endl;
    /* use 50pt at 100dpi */
    error =
        FT_Set_Char_Size(face, 1024, 1024, 100, 100); /* set character size */
    if (error) {
        std::cerr << FT_Error_String(error) << std::endl;
        return 1;
    }

    /* cmap selection omitted;                                        */
    /* for simplicity we assume that the font contains a Unicode cmap */

    slot = face->glyph;

    /* set up matrix */
    matrix.xx = (FT_Fixed)(cos(angle) * 0x10000L);
    matrix.xy = (FT_Fixed)(-sin(angle) * 0x10000L);
    matrix.yx = (FT_Fixed)(sin(angle) * 0x10000L);
    matrix.yy = (FT_Fixed)(cos(angle) * 0x10000L);

    /* the pen position in 26.6 cartesian space coordinates; */
    /* start at (300,200) relative to the upper left corner  */
    pen.x = 30 * 64;
    pen.y = (target_height - 200);

    std::cout << "Load chars" << std::endl;
    for (n = 0; n < num_chars; n++) {
        /* set transformation */
        FT_Set_Transform(face, &matrix, &pen);

        /* load glyph image into the slot (erase previous one) */
        std::cout << "Load char : " << text[n] << std::endl;
        error = FT_Load_Char(face, text[n], FT_LOAD_RENDER);
        FT_UInt glyph_index = FT_Get_Char_Index(face, text[n]);
        std::cout << "Glyph index: " << glyph_index << std::endl;

        if (error) {
            std::cerr << FT_Error_String(error) << std::endl;
            return 1;
        }
        std::cout << "Bitmap width: " << face->glyph->bitmap.width
                  << ", height: " << face->glyph->bitmap.rows << std::endl;
        std::cout << "sz:" << face->num_faces << std::endl;

        /* now, draw to our target surface (convert position) */
        draw_bitmap(&slot->bitmap, slot->bitmap_left,
                    target_height - slot->bitmap_top);

        /* increment pen position */
        pen.x += slot->advance.x;
        pen.y += slot->advance.y;
    }

    std::cout << "Show everything" << std::endl;
    show_image();

    error = FT_Done_Face(face);
    if (error) {
        std::cerr << FT_Error_String(error) << std::endl;
        return 1;
    }

    error = FT_Done_FreeType(library);
    if (error) {
        std::cerr << FT_Error_String(error) << std::endl;
        return 1;
    }

    return 0;
}

/* EOF */