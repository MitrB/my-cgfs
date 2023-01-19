#include "scene.hpp"
#include "material.hpp"

#include <cassert>
#include <random>
#include <iostream>


std::vector<Sphere> generateSpheres(Settings settings, std::mt19937 gen) {
    int N = settings.randomSphereAmount;
    if (N <= 0) {
        return {};
    }

    std::vector<Sphere> spheres{};
    std::uniform_real_distribution<float> randomRadius(settings.sphereSettings.radiusMin, settings.sphereSettings.radiusMax);
    std::uniform_real_distribution<float> randomRed(settings.sphereSettings.redMin, settings.sphereSettings.redMax);
    std::uniform_real_distribution<float> randomGreen(settings.sphereSettings.greenMin, settings.sphereSettings.greenMax);
    std::uniform_real_distribution<float> randomBlue(settings.sphereSettings.blueMin, settings.sphereSettings.blueMax);
    std::uniform_real_distribution<float> randomSpec(settings.sphereSettings.specMin, settings.sphereSettings.specMax);
    std::uniform_real_distribution<float> randomX(settings.sphereSettings.xMin, settings.sphereSettings.xMax);
    std::uniform_real_distribution<float> randomY(settings.sphereSettings.yMin, settings.sphereSettings.yMax);
    std::uniform_real_distribution<float> randomZ(settings.sphereSettings.zMin, settings.sphereSettings.zMax);

    for (int i = 0; i < N; i++) {

        // Position
        glm::vec3 position = {randomX(gen), randomY(gen), randomZ(gen)};
        position += settings.clusterOffset;

        // Radius
        float radius = randomRadius(gen);

        // Material Settings
        std::vector<std::string> materialNames = MaterialBuilder::getMaterialNames();
        assert(materialNames.size() > 0 && "0 materials loaded, aborting");
        std::uniform_int_distribution<int> randint(0, materialNames.size() - 1);
        Material material = MaterialBuilder::getMaterialProperties(materialNames[randint(gen)]);
        // random color if unvalid diffuse constant
        if (material.ambientConstant[0] < 0) {
            material.ambientConstant = glm::vec3{randomRed(gen), randomGreen(gen), randomBlue(gen)};
        }

        Sphere sphere{position, radius, material};
        spheres.push_back(sphere);
    }

    return spheres;
}

glm::vec3 generateRandomBackground(Settings settings, std::mt19937 gen) {
    std::uniform_real_distribution<float> randomRed(settings.backgroundSettings.redMin, settings.backgroundSettings.redMax);
    std::uniform_real_distribution<float> randomGreen(settings.backgroundSettings.greenMin, settings.backgroundSettings.greenMax);
    std::uniform_real_distribution<float> randomBlue(settings.backgroundSettings.blueMin, settings.backgroundSettings.blueMax);

    return glm::vec3{randomRed(gen), randomGreen(gen), randomBlue(gen)};
}

void loadPointLights(std::vector<LightDefinition> preDefinedLights, std::vector<scenario::PointLight> &lights) {
    for (LightDefinition light : preDefinedLights) {
        scenario::PointLight pointLight {light.position, light.diffusionIntensity, light.specularIntensity};
        lights.push_back(pointLight);
    }
}

void loadSpheres(std::vector<SphereDefinition> preDefinedSpheres, std::vector<Sphere> &spheres) {
    for (SphereDefinition definition : preDefinedSpheres) {
        Sphere sphere{definition.position, definition.radius, definition.material};
        spheres.push_back(sphere);
    }
}

namespace scenario {

Canvas::Canvas(std::vector<int> resolution) { this->RESOLUTION = resolution; }

Camera::Camera(glm::vec3 position) { this->position = position; }

Scene::Scene(Settings settings) {
    // Init random device
    std::random_device rd;
    std::mt19937 gen(rd());

    // Init scene
    camera = new Camera(settings.cameraPosition);
    ViewPort viewPort{};
    canvas = new Canvas(settings.resolution);

    backColor    = settings.backGroundColor;
    ambientLight = settings.ambientLight;

    if (settings.randomSpheres) {
        spheres = generateSpheres(settings, gen);   // this replaces the original spheres
    }
    if (settings.randomBackground) {
        backColor = generateRandomBackground(settings, gen);
    }

    loadSpheres(settings.preDefinedSpheres, spheres);
    loadPointLights(settings.preDefinedLights, lights);
}

Scene::~Scene() {
    delete camera;
    delete canvas;
}

}   