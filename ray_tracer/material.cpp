#include "material.hpp"
#include <string>
#include <unordered_map>
#include <vector>

std::unordered_map<std::string, Material> MaterialBuilder::materials = {};

/**
 * @brief Add material to materials
 * Never use this before loadMaterials
 * @param material
 */
void MaterialBuilder::addMaterial(Material material) { MaterialBuilder::materials[material.name] = material; }

Material MaterialBuilder::getMaterialProperties(std::string name) {
    Material material = MaterialBuilder::materials[name];
    return material;
}

void MaterialBuilder::loadMaterials() {
    // MaterialBuilder::addMaterial({"mat1", {1.f, 1.f, 1.f}, {0.5f, 0.6f, 0.3f}, glm::vec3{0.5f}, 0.9f, 0.f});
    // MaterialBuilder::addMaterial({"mat2", {.5f, .6f, .5f}, {0.9f, 0.6f, 0.3f}, glm::vec3{0.9f, 0.0f, 0.0f}, 50.f, 0.f});
    // MaterialBuilder::addMaterial({"mat3", {.1f, .1f, .1f}, {0.2f, 0.2f, 0.3f}, glm::vec3{0.5f, 0.5f, 0.9f}, .9f, 0.f});
    // MaterialBuilder::addMaterial({"mat4", {.2f, .5f, .2f}, {0.1f, 0.1f, 0.3f}, glm::vec3{0.1f, 0.0f, 0.1f}, 50.0f, 0.f});
    MaterialBuilder::addMaterial({"random_color", {1.f, 1.f, 1.f}, {0.5f, 0.6f, 0.3f}, glm::vec3{-0.5f}, 2.f, 0.0f});
    MaterialBuilder::addMaterial({"mirror", {1.f, 1.f, 1.f}, {1.f, 1.f, 1.f}, glm::vec3{1.f}, 0.9f, .9f});
}

std::vector<std::string> MaterialBuilder::getMaterialNames() {
    std::vector<std::string> keys;

    for (auto kv : materials) {
        keys.push_back(kv.first);
    }

    return keys;
}

