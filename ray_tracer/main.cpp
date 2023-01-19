#include "material.hpp"
#include "scene.hpp"

#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

#include <glm/glm.hpp>

/**
 * Returns all the spheres that lie in the line formed by the vector vec
 * @param closest should be initialized as 0
 */
std::vector<Sphere> intersectedSpheres(glm::vec3 origin, glm::vec3 direction, std::vector<Sphere> spheres, std::vector<glm::vec3> &intersectedPoints, int &closest) {
    std::vector<Sphere> intersectedSpheres{};
    // Calculate everything with the origin as 0 0 0

    float closestLength = std::numeric_limits<float>::infinity();
    int iterIndex       = 0;
    for (Sphere sphere : spheres) {
        glm::vec3 point = sphere.getPosition() - origin;
        // check if the sphere is behind the direction: discard
        if (glm::dot(direction, point) <= 0) {
            continue;
        }
        glm::vec3 projectedVector = glm::dot(direction, point) / glm::length(direction) * glm::normalize(direction);
        if (glm::distance(point, projectedVector) <= sphere.getRadius()) {

            intersectedSpheres.push_back(sphere);

            // calculate point that is closest to camera and on the intersected
            // sphere
            float length = glm::length(projectedVector) - sqrt(powf(sphere.getRadius(), 2) - powf(glm::length(projectedVector - point), 2));
            // Use this length to keep the index of the point that is the
            // overall closest to the camera
            if (length <= closestLength) {
                closest = iterIndex;   // the closest point index is the index
                                       // of the sphere that will now be added
                                       // to the sphere interesctions
                closestLength = length;
            }
            glm::vec3 intersectPoint = length * glm::normalize(direction) + origin;   // Convert the intersectionpoint to a global position
            intersectedPoints.push_back(intersectPoint);
            iterIndex++;
        }
    }
    return intersectedSpheres;
}

void renderScene(scenario::Scene *scene, std::vector<std::vector<float>> &buffer) {
    // Precalc
    std::vector<int> resolution = scene->canvas->getResolution();
    int width                   = resolution[0];
    int height                  = resolution[1];
    float viewPortWidth         = scene->viewPort.getRUP()[0] - scene->viewPort.getLDP()[0];
    float viewPortHeight        = -scene->viewPort.getRUP()[1] + scene->viewPort.getLDP()[1];
    float pixelWidth            = viewPortWidth / resolution[0];
    float pixelHeight           = viewPortHeight / resolution[1];

    // Iterate over every pixel
    for (size_t j = 0; j < height; j++) {
        for (size_t i = 0; i < width; i++) {
            glm::vec3 color = scene->backColor;

            std::vector<glm::vec3> intersectedPoints{};
            int closest = 0;

            glm::vec3 direction =
                glm::vec3{pixelWidth * i - viewPortWidth / 2 + pixelWidth / 2, pixelHeight * j - viewPortHeight / 2 + pixelHeight / 2, scene->viewPort.getZ()};   // this is relative to the camera
            std::vector<Sphere> intersected = intersectedSpheres(scene->camera->getPosition(), direction, scene->spheres, intersectedPoints, closest);

            if (intersected.size() > 0) {

                // calculate light
                glm::vec3 ambientLight  = scene->ambientLight * intersected[closest].getAmbientConstant();
                glm::vec3 diffuseLight  = glm::vec3{.0f};
                glm::vec3 specularLight = glm::vec3{0.f};
                for (scenario::PointLight light : scene->lights) {
                    // If blocked by another sphere: skip, this is shadow
                    int _c = 0;
                    if (intersectedSpheres(intersectedPoints[closest], light.position - intersectedPoints[closest], scene->spheres, intersectedPoints, _c).size() > 0) {
                        continue;
                    }

                    glm::vec3 lightDir     = glm::normalize(light.position - intersectedPoints[closest]);
                    glm::vec3 normalVector = glm::normalize(intersectedPoints[closest] - intersected[closest].getPosition());

                    // Diffuse reflection
                    diffuseLight += intersected[closest].getDiffuseConstant() * light.diffusionIntensity * std::max(0.f, glm::dot(normalVector, lightDir));

                    // Specular reflection
                    glm::vec3 lightBounceDir = 2 * glm::dot(lightDir, normalVector) * normalVector - lightDir;
                    specularLight += intersected[closest].getSpecularityConstant() * light.specularIntensity *
                                     powf(std::max(0.f, glm::dot(-(direction + scene->camera->getPosition()), lightBounceDir)), intersected[closest].getShineFactor());
                }
                color = ambientLight + diffuseLight + specularLight;
            }

            std::vector<float> colorWritable{};
            for (int i = 0; i < 3; i++) {
                colorWritable.push_back(color[i]);
            }

            buffer.push_back(colorWritable);
        }
    }
}

void render(int width, int height, std::vector<std::vector<float>> &framebuffer) {

    std::ofstream ofs;   // save the framebuffer to file
    ofs.open("./out.ppm");
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (size_t i = 0; i < height * width; ++i) {
        for (size_t j = 0; j < 3; j++) {
            ofs << (char) (255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
        }
    }
    ofs.close();
}

void configureSettings(Settings& settings) {
    // Hardcoded predefined spheres and lights
    // SphereDefinition sphere{{-1.0f, -0.5f, 5.0f}, 1.f, MaterialBuilder::getMaterialProperties("mat1")};
    // SphereDefinition sphere2{{0.0f, 0.0f, 5.0f}, 1.f, MaterialBuilder::getMaterialProperties("mat2")};
    // SphereDefinition sphere3{{1.0f, +0.5f, 5.0f}, 1.f, MaterialBuilder::getMaterialProperties("mat3")};
    // settings.preDefinedSpheres = {sphere, sphere2, sphere3};
    settings.randomSpheres    = true;
    settings.randomBackground = true;
    LightDefinition pointLight{{0.f, -10.f, 3.f}, glm::vec3{1.f}, glm::vec3{1.f}};
    settings.preDefinedLights = {pointLight};
}

int main() {
    // Load settings
    Settings settings{};
    MaterialBuilder::loadMaterials();

    configureSettings(settings);

    scenario::Scene *scene = new scenario::Scene(settings);

    // buffer for writing scene to file
    std::vector<std::vector<float>> framebuffer{};

    // debug info
    if (settings.debug) {
        std::cout << "Scene succesfully build."<< '\n';
        std::cout << "Rendering: "<< '\n';
        std::cout << '\t' << "Spheres #: " << scene->spheres.size() << '\n';
        std::cout << '\t' << "Lights #: " << scene->lights.size() << '\n';
    }

    renderScene(scene, framebuffer);
    if (settings.debug) {
        std::cout << "Scene succesfully rendered."<< '\n';
        std::cout << "Writing "<< framebuffer.size() << " pixels." << '\n';
    }
    render(scene->canvas->getResolution()[0], scene->canvas->getResolution()[1], framebuffer);

    return 0;
}
