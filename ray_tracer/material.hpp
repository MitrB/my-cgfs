#pragma once

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace material {
struct Material {
    std::string name;
    glm::vec3 specularConstant;
    glm::vec3 diffuseConstant;
    glm::vec3 ambientConstant;
    float shineFactor;
    float reflectionFraction; // 1.f will be a perfect mirror
};

    void loadMaterials();

    void addMaterial(Material material);
    Material getMaterialProperties(std::string name);

    std::vector<std::string> getMaterialNames();

    static std::unordered_map<std::string, Material> materials{};
};
