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

namespace learning {

unsigned int textureFromFile(const char* path, const std::string& directory,
                             bool gamma = false);

class Model {
   public:
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;

    Model(const std::string& path, bool gamma = false);
    void draw(Shader& shader);

   private:
    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

}  // namespace learning

#endif  // MODEL_H