#include "material.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace material {

/**
 * @brief Add material to materials
 * Never use this before loadMaterials
 * @param material
 */
void addMaterial(Material material) { materials.emplace(material.name, material); }

Material getMaterialProperties(std::string name) {
    Material material = materials[name];
    return material;
}

void loadMaterials() {
    addMaterial({"mat1", {1.f, 1.f, 1.f}, {0.5f, 0.6f, 0.3f}, glm::vec3{0.5f}, 0.9f, 0.f});
    addMaterial({"mat2", {.5f, .6f, .5f}, {0.9f, 0.6f, 0.3f}, glm::vec3{0.9f, 0.0f, 0.0f}, 50.f, 0.f});
    addMaterial({"mat3", {.1f, .1f, .1f}, {0.2f, 0.2f, 0.3f}, glm::vec3{0.5f, 0.5f, 0.9f}, .9f, 0.f});
    addMaterial({"mat4", {.2f, .5f, .2f}, {0.1f, 0.1f, 0.3f}, glm::vec3{0.1f, 0.0f, 0.1f}, 50.0f, 0.f});
    addMaterial({"random_color", {1.f, 1.f, 1.f}, {0.5f, 0.6f, 0.3f}, glm::vec3{-0.5f}, 2.f, 0.0f});
    addMaterial({"mirror", {1.f, 1.f, 1.f}, {1.f, 1.f, 1.f}, glm::vec3{1.f}, 0.9f, .9f});
    addMaterial({"random_color_mirror", {1.f, 1.f, 1.f}, {0.9f, 0.9f, 0.9f}, glm::vec3{-0.5f}, 20.f, 0.1f});
}

std::vector<std::string> getMaterialNames() {
    std::vector<std::string> keys;

    for (auto kv : materials) {
        keys.push_back(kv.first);
    }

    return keys;
}

}
