#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include "mesh.h"


struct Texture {
    unsigned int id;
    std::string type;
    std::string path;

    Texture() {};
    Texture(const char* filename, const std::string& typeName) {
        load(filename, typeName);
    }
    void load(const char* filename, const std::string& typeName) {
        id = TextureFromFile(filename);
        type = typeName;
        path = filename;
    }

    unsigned int TextureFromFile(const char* path)
    {
        std::string filename = std::string(path);
        std::cout << "Texture path: " << path << std::endl;

        unsigned int textureID = 0;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            printf("texture from file\n");
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }

};


