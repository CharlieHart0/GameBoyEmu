#pragma once
#include <SDL_opengl.h>
#include <imgui.h>
#include <cstdint>
#include "GBPallette.h"
#ifndef STB_IMAGE_IMPLEMENTATION
    #define STB_IMAGE_IMPLEMENTATION
    #include "../GameBoyEmu/Vendor/stb/stb_image.h"
#endif

// Simple helper function to load an image into a OpenGL texture with common settings
bool LoadTextureFromMemory(const void* data, size_t data_size, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load_from_memory((const unsigned char*) data, (int) data_size, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Upload pixels into texture
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;
    
    return true;
}

// Open and read a file, then forward to LoadTextureFromMemory()
bool LoadTextureFromFile(const char* file_name, GLuint* out_texture, int* out_width, int* out_height)
{
    FILE* f = fopen(file_name, "rb");
    if (f == NULL)
        return false;
    fseek(f, 0, SEEK_END);
    size_t file_size = (size_t) ftell(f);
    if (file_size == -1)
        return false;
    fseek(f, 0, SEEK_SET);
    void* file_data = IM_ALLOC(file_size);
    fread(file_data, 1, file_size, f);
    bool ret = LoadTextureFromMemory(file_data, file_size, out_texture, out_width, out_height);
    IM_FREE(file_data);
    return ret;
}

class ImGuiTexture
{
public:
    int mImgWidth = 0;
    int mImgHeight = 0;
    GLuint mImgTex = 0;

    void deleteTexture()
    {
        glDeleteTextures(1, &mImgTex);
    }

    ImGuiTexture(){}

    ImGuiTexture(const char* file_name)
    {
       bool ret = LoadTextureFromFile(file_name, &mImgTex, &mImgWidth, &mImgHeight);
       IM_ASSERT(ret);
    }

    ImGuiTexture(uint8_t* data, int image_width, int image_height, GBPalette* pal = nullptr)
    {
        void* image_data = data;

        if (pal != nullptr)
        {
            // convert the colours

            image_data = IM_ALLOC(image_height * image_width * 4);
            
            ColoursFromPallette8x8((uint8_t*)image_data, data, pal);
        }

        // Create a OpenGL texture identifier
        GLuint image_texture;
        glGenTextures(1, &image_texture);
        glBindTexture(GL_TEXTURE_2D, image_texture);

        // Setup filtering parameters for display
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Upload pixels into texture
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        

        if (pal != nullptr)
        {
            IM_FREE(image_data);
        }
        else
        {
            stbi_image_free(image_data);
        }

        mImgTex = image_texture;
        mImgWidth = image_width;
        mImgHeight = image_height;
    }

protected:
    void ColoursFromPallette8x8(uint8_t* coldata, uint8_t* indexdata, GBPalette* pal)
    {
        for (int row = 0; row < 8; row++)
        {
            uint8_t* firstbyte = indexdata + (2 * row);
            uint8_t* secondbyte = indexdata + (2 * row) + 1;
            for (int column = 0; column < 8; column++)
            {
                // msb is leftmost colour
                uint8_t ind = ((*firstbyte >> (8-column)) & 0b1) + ((*secondbyte >> (8-column)) & 0b1)*2;
                IM_ASSERT(ind < 4);
                int offset = ((row * 8) + column)*4;
                uint8_t* colourlocation = coldata + offset;
                std::copy(&(pal->colours[ind][0]), &(pal->colours[ind][3]), colourlocation);
            }
        }
    }
};




