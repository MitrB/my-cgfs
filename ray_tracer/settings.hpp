#pragma once

#include "material.hpp"
#include <vector>

#include <glm/glm.hpp>

struct SphereDefinition {
    glm::vec3 position;
    float radius;
    material::Material material;
};

struct LightDefinition {
    glm::vec3 position;
    glm::vec3 diffusionIntensity;
    glm::vec3 specularIntensity;
};

struct RandomSphereSettings {
    float xMin = -5.f;
    float xMax = 5.f;
    float yMin = -5.f;
    float yMax = 5.f;
    float zMin = -5.f;
    float zMax = 5.f;

    float radiusMin = 0.f;
    float radiusMax = 1.f;

    float redMin   = 0.f;
    float greenMin = 0.f;
    float blueMin  = 0.f;
    float redMax   = 1.f;
    float greenMax = 1.f;
    float blueMax  = 1.f;

    float specMin = 0.f;
    float specMax = 1.f;
};

struct RandomBackGroundSettings {
    float redMin   = 0.f;
    float greenMin = 0.f;
    float blueMin  = 0.f;
    float redMax   = 1.f;
    float greenMax = 1.f;
    float blueMax  = 1.f;
};

struct Settings {
    std::vector<int> resolution{1080, 1080};
    glm::vec3 cameraPosition{0.f, 0.f, -1.f};

    bool randomSpheres{true};
    int randomSphereAmount{100};
    RandomSphereSettings sphereSettings{};
    glm::vec3 clusterOffset{0.f, 0.f, 10.f};

    bool randomBackground{true};
    RandomBackGroundSettings backgroundSettings{};
    glm::vec3 backGroundColor{0.3f, 0.3f, 0.9f};

    glm::vec3 ambientLight{.5f};

    std::vector<SphereDefinition> preDefinedSpheres{};
    std::vector<LightDefinition> preDefinedLights{};

    int reflectionCount = 3;

    bool debug = true;
};