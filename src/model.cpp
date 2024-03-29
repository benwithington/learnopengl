#include "model.h"

#include "stb_image.h"

namespace personal::renderer::utility {

RawModel::RawModel(std::vector<float>& positions, std::vector<float>& texCoords)
    : numTriangles(positions.size()) {
    // Generate Vertex Array Object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    /*
    // Generate the Vertex Buffer Object for the positions
    unsigned int positionsBuffer;
    glGenBuffers(1, &positionsBuffer);
    vbos.push_back(positionsBuffer);
    GLuint positionsIndex = 0;
    glBindBuffer(GL_ARRAY_BUFFER, positionsBuffer);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float),
    &positions[0], GL_STATIC_DRAW); glVertexAttribPointer(positionsIndex, 3,
    GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Generate the Vertex Buffer Object for the tex coords
    unsigned int texCoordBuffer;
    glGenBuffers(1, &texCoordBuffer);
    vbos.push_back(texCoordBuffer);
    GLuint texCoordsIndex = 1;
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float),
                 &texCoords[0], GL_STATIC_DRAW);
    glVertexAttribPointer(texCoordsIndex, 2, GL_FLOAT, GL_FALSE,
                          2 * sizeof(float), (void*)0);

    // Enable attrib arrays
    glEnableVertexAttribArray(positionsIndex);
    glEnableVertexAttribArray(texCoordsIndex);
    */

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER,
                 (positions.size() + texCoords.size()) * sizeof(float), nullptr,
                 GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(float),
                    &positions[0]);
    glBufferSubData(GL_ARRAY_BUFFER, positions.size() * sizeof(float),
                    texCoords.size() * sizeof(float), &texCoords[0]);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                          (void*)(positions.size() * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Unbind the VAO, resetting to default state
    glBindVertexArray(0);
}

RawModel::RawModel(std::vector<float>& positions, std::vector<float>& texCoords,
                   std::vector<float>& normals)
    : numTriangles(positions.size()) {
    // Generate Vertex Array Object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Generate the Vertex Buffer Object for the positions
    unsigned int positionsBuffer;
    glGenBuffers(1, &positionsBuffer);
    vbos.push_back(positionsBuffer);
    GLuint positionsIndex = 0;
    glBindBuffer(GL_ARRAY_BUFFER, positionsBuffer);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float),
                 &positions[0], GL_STATIC_DRAW);
    glVertexAttribPointer(positionsIndex, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(float), (void*)0);

    // Generate the Vertex Buffer Object for the tex coords
    unsigned int texCoordBuffer;
    glGenBuffers(1, &texCoordBuffer);
    vbos.push_back(texCoordBuffer);
    GLuint texCoordsIndex = 1;
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float),
                 &texCoords[0], GL_STATIC_DRAW);
    glVertexAttribPointer(texCoordsIndex, 2, GL_FLOAT, GL_FALSE,
                          2 * sizeof(float), (void*)0);

    // Generate the vertex buffer object for the normals
    unsigned int normalBuffer;
    glGenBuffers(1, &normalBuffer);
    vbos.push_back(normalBuffer);
    GLuint normalsIndex = 2;
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0],
                 GL_STATIC_DRAW);
    glVertexAttribPointer(normalsIndex, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(float), (void*)0);

    // Enable attrib arrays
    glEnableVertexAttribArray(positionsIndex);
    glEnableVertexAttribArray(texCoordsIndex);
    glEnableVertexAttribArray(normalsIndex);

    // Unbind the VAO, resetting to default state
    glBindVertexArray(0);
}

RawModel::~RawModel() {
    glDeleteVertexArrays(1, &vao);
    for (auto b : vbos) glDeleteBuffers(1, &b);
}

void RawModel::draw(const Shader&) const {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(numTriangles));
    glBindVertexArray(0);
}

AssimpModel::AssimpModel(const std::string& path, bool gamma)
    : gammaCorrection(gamma) {
    loadModel(path);
}

// draws the model, and thus all its meshes
void AssimpModel::draw(const Shader& shader) const {
    for (unsigned int i = 0; i < meshes.size(); i++) meshes[i].draw(shader);
}

// loads a model with supported ASSIMP extensions from file and stores the
// resulting meshes in the meshes vector.
void AssimpModel::loadModel(const std::string& path) {
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                  aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode)  // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << '\n';
        return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);
}

// processes a node in a recursive fashion. Processes each individual mesh
// located at the node and repeats this process on its children nodes (if any).
void AssimpModel::processNode(aiNode* node, const aiScene* scene) {
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        // the node object only contains indices to index the actual objects in
        // the scene. the scene contains all the data, node is just to keep
        // stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively
    // process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh AssimpModel::processMesh(aiMesh* mesh, const aiScene* scene) {
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 vector;  // we declare a placeholder vector since assimp uses
                           // its own vector class that doesn't directly convert
                           // to glm's vec3 class so we transfer the data to
                           // this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;
        // normals
        if (mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0])  // does the mesh contain texture
                                      // coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We
            // thus make the assumption that we won't use models where a vertex
            // can have multiple texture coordinates so we always take the first
            // set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.bitangent = vector;
        } else
            vertex.texCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle)
    // and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse
    // texture should be named as 'texture_diffuseN' where N is a sequential
    // number ranging from 1 to MAX_SAMPLER_NUMBER. Same applies to other
    // texture as the following list summarizes: diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    std::vector<Texture> diffuseMaps = loadMaterialTextures(
        material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<Texture> specularMaps = loadMaterialTextures(
        material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps =
        loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps =
        loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, textures);
}

// checks all material textures of a given type and loads the textures if
// they're not loaded yet. the required info is returned as a Texture struct.
std::vector<Texture> AssimpModel::loadMaterialTextures(aiMaterial* mat,
                                                       aiTextureType type,
                                                       std::string typeName) {
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next
        // iteration: skip loading a new texture
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); ++j) {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip =
                    true;  // a texture with the same filepath has already been
                           // loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip) {  // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = textureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(
                texture);  // store it as texture loaded for entire model, to
                           // ensure we won't unnecessary load duplicate
                           // textures.
        }
    }
    return textures;
}

unsigned int textureFromFile(const char* path, const std::string& directory,
                             [[maybe_unused]] bool gamma) {
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data =
        stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format{};
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

};  // namespace personal::renderer::utility