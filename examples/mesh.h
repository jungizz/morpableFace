#pragma once

#include <iostream>
#define GLEW_STATIC 
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>
#include <vector>
#include "toys.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <Eigen/Dense>
#include <Eigen/SVD>

#include "texture.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    float weight = 1.0f;
};

static const glm::vec3 gravity = glm::vec3(0.0f, -9.8f, 0.0f); 

extern Program program;

extern glm::vec3 contact_point = glm::vec3(0.0f, -600.0f, 0.0f); 
extern glm::vec3 normal_vector = normalize(glm::vec3(0.2f, 1.0f, 0.1f));


struct Mesh {
    
    GLuint vertexBuffer = 0;
    GLuint vertexArray = 0;
    GLuint elementBuffer = 0;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    std::vector<glm::vec3> velocity;


    glm::vec3 t_0 = glm::vec3(0.0f);
    std::vector<glm::vec3> q_values;  // q 벡터:  초기모양 - t_0 
    std::vector<float> weight_values;  // weights 벡터의 값들... vertices.weight
    std::vector<glm::vec3> origin_point = std::vector<glm::vec3>(0);

    glm::vec3 init_height = glm::vec3(0.0f, 600.0f, 0.0f);
    
    //constructor
    Mesh(const std::vector<Vertex>& _vertices, const std::vector<unsigned int>& _indices, const std::vector<Texture>& _textures) :vertices(_vertices), indices(_indices), textures(_textures) {

    }


    void setupMesh() {
        //vertexBuffer
        glGenVertexArrays(1, &vertexArray);
        glGenBuffers(1, &vertexBuffer);
        glGenBuffers(1, &elementBuffer);

        glBindVertexArray(vertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        glBindVertexArray(0);
    }
    void render() {
        if (vertexBuffer == 0) setupMesh();

        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;

        GLuint TexOrColorLocation = glGetUniformLocation(program.programID, "TexOrColor");
        glUniform1i(TexOrColorLocation, 1); // true이면 1, false이면 0

        
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            std::string number;
            std::string name = textures[i].type;
            if (name == "diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "specular")
                number = std::to_string(specularNr++);
            else if (name == "normal")
                number = std::to_string(normalNr++);
            else if (name == "height")
                number = std::to_string(heightNr++);

            glUniform1i(glGetUniformLocation(program.programID, (name + number).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        glBindVertexArray(vertexArray);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }
};
