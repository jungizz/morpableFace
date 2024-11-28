
#include "mesh.h"
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>


//주어진 texture 위치들에 대해 반복문을 돌리고 
//파일의 위치를 얻은 다음 texture 생성
//Vertex struct에 저장

inline Mesh processMesh(aiMesh* mesh, const aiScene* scene, std::vector<Texture>& textures_loaded);


inline std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::vector<Texture>& textures_loaded)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        //중복 제거
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (textures_loaded[j].path.compare(str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {
            textures.emplace_back(str.C_Str(), typeName);
            textures_loaded.push_back(textures.back());
        }
    }
    return textures;
}

inline void processNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes, std::vector<Texture>& textures_loaded) {
    //now
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene,textures_loaded));
    }
    //recursive
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, meshes,textures_loaded);
    }
}

inline Mesh processMesh(aiMesh* mesh, const aiScene* scene, std::vector<Texture>& textures_loaded) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    //stbi_set_flip_vertically_on_load(true); //dae파일은 //처리...

    // 메시의 정점 데이터 처리
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex; //vertex structure -> position, normal, texcoord

        //position
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        //normal
        if (mesh->HasNormals())
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

        //texcoord
        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }
    // 인덱스 데이터 처리
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    //aimaterial => index만  (mMaterials에 index 설정)
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // Diffuse maps
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse", textures_loaded);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        // Specular maps
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular", textures_loaded);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        // Normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "normal", textures_loaded);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        // Height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "height", textures_loaded);
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        return Mesh(vertices, indices, textures);
    }

}

inline std::vector<Mesh> loadMesh(const std::filesystem::path& file) {
    Assimp::Importer importer;
    const aiScene* scene;
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    scene = importer.ReadFile(file.string().c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
    program.loadShaders("shader.vert", "shader.frag");
    processNode(scene->mRootNode, scene, meshes, textures_loaded);
    return meshes;
}
#pragma once
