#pragma once

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>
#include <vector>

struct Material {
    std::string name;
    glm::vec3 specularConstant;
    glm::vec3 diffuseConstant;
    glm::vec3 ambientConstant;
    float shineFactor;
    float reflectionFraction; // 1.f will be a perfect mirror
};

class MaterialBuilder {
  public:
    MaterialBuilder();
    static void loadMaterials();

    static void addMaterial(Material material);
    static Material getMaterialProperties(std::string name);

    static std::vector<std::string> getMaterialNames();

    static std::unordered_map<std::string, Material> materials;
};
