#ifndef MODEL_H
#define MODEL_H

#include "GL/glew.h"

#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "stb_image.h"


#include "mesh.h"
#include "shader.h"
#include "Camera.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;

inline unsigned int TextureFromFile(const char* path);

class Model
{
public:
    vector<Texture> textures;
    Mesh* mesh = nullptr;

    Model(vector<const char*> texturesPath)
    {
        loadModel(texturesPath);
    }

    ~Model()
    {
        if (mesh)
            delete mesh;
    }

    void Draw(Shader& shader, Camera& camera)
    {
        mesh->Draw(shader, camera);
    }

    void setTransform(glm::mat4 transform)
    {
        mesh->setTransform(transform);
    }

private:
    void loadModel(vector<const char*> texturesPath)
    {
        vector<Texture> textures;
        for (int i = 1; i <= texturesPath.size(); ++i)
        {
            Texture texture;
            texture.id = TextureFromFile(texturesPath[i - 1]);
            texture.path = texturesPath[i - 1];
            texture.type = "texture" + to_string(i);
            textures.push_back(texture);
        }

        mesh = new Mesh(textures);
    }
};

inline unsigned int TextureFromFile(const char* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

#endif