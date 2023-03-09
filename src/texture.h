#ifndef TEXTURE_H
#define TEXTIRE_H

#include <string>
#include <vector>

namespace personal::renderer::utility {
unsigned int loadTexture(std::string path);
unsigned int loadCubemap(std::string path, std::vector<std::string> faces);
}  // namespace personal::renderer::utility

#endif  // TEXTURE_H
