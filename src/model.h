#ifndef MODEL_H
#define MODEL_H

/* clang-format off */
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>

#include "stb_image.h"

#include "mesh.h"
/* clang-format on  */

namespace personal::renderer::utility {

unsigned int textureFromFile(const char* path, const std::string& directory,
                             bool gamma = false);

class Model {
    public:
        virtual void draw(const Shader& shader) const = 0;
};

class RawModel : public Model {
    public:
        RawModel(std::vector<float>& vertices, std::vector<float> texCoords);
        ~RawModel();
        void draw(const Shader& shader) const override;
    private:
        unsigned int vao;
        unsigned int verticesBuffer;
        unsigned int texCoordBuffer;
        std::size_t numTriangles;
};

class AssimpModel : public Model {
   public:
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;

    AssimpModel(const std::string& path, bool gamma = false);
    void draw(const Shader& shader) const override;

   private:
    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

}  // namespace learning

#endif  // MODEL_H