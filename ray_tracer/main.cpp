#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

#include <glm/glm.hpp>

/* Coordinate system:
 * The middle of the screen should be (0, 0)
 * X grows positivly to the right
 * Y grows negativly down
 * Z grows positivly in the direction through the screen
 */
namespace rayTracer {

class Canvas {
  public:
    Canvas(std::vector<int> resolution);
    ~Canvas(){};

    std::vector<int> getResolution() { return RESOLUTION; }

  private:
    std::vector<int> RESOLUTION{640, 640};   // WIDHT HEIGHT
};

Canvas::Canvas(std::vector<int> resolution) { this->RESOLUTION = resolution; }
class ViewPort {
    // The viewport is relative to the camera
  public:
    ViewPort(){};
    ~ViewPort(){};

    float getZ() { return this->LEFT_DOWN_POS[2]; }
    glm::vec3 getLDP() { return LEFT_DOWN_POS; }
    glm::vec3 getRUP() { return RIGHT_UP_POS; }

  private:
    /* +--------RUP
     * |         |
     * LDP-------+
     */
    glm::vec3 LEFT_DOWN_POS{-.5f, .5f, 1.f};   // remember the Y axis grows downwards
    glm::vec3 RIGHT_UP_POS{.5f, -.5f, 1.f};
};

class Camera {
  public:
    Camera(glm::vec3 position);
    ~Camera(){};

    glm::vec3 getPosition() { return this->position; }

  private:
    glm::vec3 position{0.0f, 0.0f, 0.0f};
};

Camera::Camera(glm::vec3 position) { this->position = position; }

class Sphere {
  public:
    Sphere(glm::vec3 position, float radius, glm::vec3 color, float specularity);
    ~Sphere(){};

    glm::vec3 getPosition() { return this->position; }
    float getRadius() { return this->radius; }
    glm::vec3 getColor() { return this->color; }
    float getSpecularity() { return this->specularity; }

  private:
    glm::vec3 position;
    float radius;
    glm::vec3 color;
    float specularity;
};

Sphere::Sphere(glm::vec3 position, float radius, glm::vec3 color, float specularity) {
    this->position    = position;
    this->radius      = radius;
    this->color       = color;
    this->specularity = specularity;
}

struct PointLight {
    glm::vec3 position;
    float intensity;
};

}   // namespace rayTracer

struct SphereDefinition {
    glm::vec3 position;
    float radius;
    glm::vec3 color;
    float specularity;
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
    std::vector<int> resolution{640, 640};
    glm::vec3 cameraPosition{0.f, 0.f, -1.f};

    bool randomSpheres{true};
    int randomSphereAmount{10};
    RandomSphereSettings sphereSettings{};
    glm::vec3 clusterOffset{0.f, 0.f, 10.f};

    bool randomBackground{true};
    RandomBackGroundSettings backgroundSettings{};
    glm::vec3 backGroundColor{0.2f, 0.4f, 0.4f};

    glm::vec3 ambientLight{.4f};

    std::vector<SphereDefinition> preDefinedSpheres{};
    std::vector<rayTracer::PointLight> preDefinedLights{};

    bool debug = true;
};

/**
 * Returns all the spheres that lie in the line formed by the vector vec
 * @param closest should be initialized as 0
 */
std::vector<rayTracer::Sphere> intersectedSpheres(glm::vec3 origin, glm::vec3 direction, std::vector<rayTracer::Sphere> spheres, std::vector<glm::vec3> &intersectedPoints, int &closest) {
    std::vector<rayTracer::Sphere> intersectedSpheres{};
    // Calculate everything with the origin as 0 0 0

    float closestLength = std::numeric_limits<float>::infinity();
    int iterIndex       = 0;
    for (rayTracer::Sphere sphere : spheres) {
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

std::vector<rayTracer::Sphere> generateSpheres(Settings settings, std::mt19937 gen) {
    int N = settings.randomSphereAmount;
    if (N <= 0) {
        return {};
    }

    std::vector<rayTracer::Sphere> spheres{};
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
        glm::vec3 color = {randomRed(gen), randomGreen(gen), randomBlue(gen)};
        float spec      = randomSpec(gen);

        rayTracer::Sphere sphere{position, radius, color, spec};
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

void loadPointLights(std::vector<rayTracer::PointLight> preDefinedLights, std::vector<rayTracer::PointLight>& lights) {
    for (rayTracer::PointLight light : preDefinedLights) {
        lights.push_back(light);
    }
}

void loadSpheres(std::vector<SphereDefinition> preDefinedSpheres, std::vector<rayTracer::Sphere> &spheres) {
    for (SphereDefinition definition : preDefinedSpheres) {
        rayTracer::Sphere sphere{definition.position, definition.radius, definition.color, definition.specularity};
        spheres.push_back(sphere);
    }
}

int main() {
    // Load settings
    Settings settings{};
    // Hardcoded predefined spheres and lights
    // SphereDefinition sphere{{-1.0f, -0.5f, 5.0f}, 1.f, {.9f, .5f, .2f}, 1.0f};
    // SphereDefinition sphere2{{0.0f, 0.0f, 5.0f}, 1.f, {.2f, .9f, .2f}, 0.1f};
    // SphereDefinition sphere3{{1.0f, +0.5f, 5.0f}, 1.f, {.2f, .1f, .7f}, 0.5f};
    rayTracer::PointLight pointLight{{0.f, -10.f, 3.f}, 1.f};
    settings.randomSpheres    = true;
    settings.randomBackground = true;

    // settings.preDefinedSpheres = {sphere, sphere2, sphere3};
    settings.preDefinedLights  = {pointLight};

    // Init random device
    std::random_device rd;
    std::mt19937 gen(rd());

    // Init scene
    rayTracer::Camera camera{settings.cameraPosition};
    rayTracer::ViewPort viewPort{};
    rayTracer::Canvas canvas{settings.resolution};

    std::vector<rayTracer::Sphere> spheres{};
    std::vector<rayTracer::PointLight> lights{};
    glm::vec3 backColor = settings.backGroundColor;
    glm::vec3 ambientLight = settings.ambientLight;

    if (settings.randomSpheres) {
        spheres = generateSpheres(settings, gen);   // this replaces the original spheres
    }
    if (settings.randomBackground) {
        backColor = generateRandomBackground(settings, gen);
    }

    loadSpheres(settings.preDefinedSpheres, spheres);
    loadPointLights(settings.preDefinedLights, lights);

    // Precalc
    std::vector<int> resolution = canvas.getResolution();
    int width                   = resolution[0];
    int height                  = resolution[1];
    float viewPortWidth         = viewPort.getRUP()[0] - viewPort.getLDP()[0];
    float viewPortHeight        = -viewPort.getRUP()[1] + viewPort.getLDP()[1];
    float pixelWidth            = viewPortWidth / resolution[0];
    float pixelHeight           = viewPortHeight / resolution[1];

    // buffer for writing scene to file
    std::vector<std::vector<float>> framebuffer{};

    // debug info
    if (settings.debug) {
        std::cout << "Spheres #: "<< spheres.size() << '\n';
        std::cout << "Lights #: "<< lights.size() << '\n';
    }

    // Iterate over every pixel
    for (size_t j = 0; j < height; j++) {
        for (size_t i = 0; i < width; i++) {
            glm::vec3 color = backColor;

            std::vector<glm::vec3> intersectedPoints{};
            int closest = 0;

            glm::vec3 direction =
                glm::vec3{pixelWidth * i - viewPortWidth / 2 + pixelWidth / 2, pixelHeight * j - viewPortHeight / 2 + pixelHeight / 2, viewPort.getZ()};   // this is relative to the camera
            std::vector<rayTracer::Sphere> intersected = intersectedSpheres(camera.getPosition(), direction, spheres, intersectedPoints, closest);

            if (intersected.size() > 0) {

                // base sphere color
                color = intersected[closest].getColor();

                // calculate light
                float diffuseLight  = 0;
                float specularLight = 0;
                for (rayTracer::PointLight light : lights) {
                    // If blocked by another sphere: skip, this is shadow
                    int _c = 0;
                    if (intersectedSpheres(intersectedPoints[closest], light.position - intersectedPoints[closest], spheres, intersectedPoints, _c).size() > 0) {
                        continue;
                    }

                    glm::vec3 lightDir     = glm::normalize(light.position - intersectedPoints[closest]);
                    glm::vec3 normalVector = glm::normalize(intersectedPoints[closest] - intersected[closest].getPosition());

                    // diffuse reflection
                    diffuseLight += light.intensity * std::max(0.f, glm::dot(normalVector, lightDir));

                    // specular reflection
                    glm::vec3 lightBounceDir = 2 * glm::dot(lightDir, normalVector) * normalVector - lightDir;
                    float specularity        = intersected[closest].getSpecularity();
                    specularLight += light.intensity * specularity * std::max(0.f, glm::dot(-(direction + camera.getPosition()), lightBounceDir));
                }
                color = ambientLight * color + diffuseLight * color + specularLight * color;   // can play around with this
            }

            std::vector<float> colorWritable{};
            for (int i = 0; i < 3; i++) {
                colorWritable.push_back(color[i]);
            }

            framebuffer.push_back(colorWritable);
        }
    }

    render(width, height, framebuffer);

    return 0;
}
