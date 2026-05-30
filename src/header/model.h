#ifndef MODEL_H
#define MODEL_H

#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <QVector2D>
#include <QImage>
#include <QDebug>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

class Model
{
public:
    std::vector<Texture> textures_loaded;
    std::vector<Mesh>    meshes;
    std::string directory;
    bool gammaCorrection;

    Model(const std::string &path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
    }

    void Draw(QOpenGLShaderProgram &shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:
    void loadModel(const std::string &path)
    {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            qWarning() << "ERROR::ASSIMP::" << importer.GetErrorString();
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode *node, const aiScene *scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            QVector3D vector;
            // positions
            vector.setX(mesh->mVertices[i].x);
            vector.setY(mesh->mVertices[i].y);
            vector.setZ(mesh->mVertices[i].z);
            vertex.Position = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.setX(mesh->mNormals[i].x);
                vector.setY(mesh->mNormals[i].y);
                vector.setZ(mesh->mNormals[i].z);
                vertex.Normal = vector;
            }
            // texture coordinates
            if (mesh->mTextureCoords[0])
            {
                vertex.TexCoords = QVector2D(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
                // tangent
                vector.setX(mesh->mTangents[i].x);
                vector.setY(mesh->mTangents[i].y);
                vector.setZ(mesh->mTangents[i].z);
                vertex.Tangent = vector;
                // bitangent
                vector.setX(mesh->mBitangents[i].x);
                vector.setY(mesh->mBitangents[i].y);
                vector.setZ(mesh->mBitangents[i].z);
                vertex.Bitangent = vector;
            }
            else
            {
                vertex.TexCoords = QVector2D(0.0f, 0.0f);
            }
            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        return Mesh(vertices, indices, textures);
    }

    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName)
    {
        std::vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.c_str(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip)
            {
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        return textures;
    }
};

inline unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma)
{
    Q_UNUSED(gamma);

    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    QImage image(QString::fromStdString(filename));
    if (image.isNull())
    {
        qWarning() << "Texture failed to load at path:" << path;
        return 0;
    }

    // Flip vertically — equivalent to stbi_set_flip_vertically_on_load(true)
    image = image.mirrored(false, true);

    // Detect channel count and choose matching OpenGL format
    GLenum format;
    QImage::Format targetFormat;
    if (image.isGrayscale())
    {
        format = GL_RED;
        targetFormat = QImage::Format_Grayscale8;
    }
    else if (image.hasAlphaChannel())
    {
        format = GL_RGBA;
        targetFormat = QImage::Format_RGBA8888;
    }
    else
    {
        format = GL_RGB;
        targetFormat = QImage::Format_RGB888;
    }
    image = image.convertToFormat(targetFormat);

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, format, image.width(), image.height(),
                 0, format, GL_UNSIGNED_BYTE, image.bits());
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}

#endif
