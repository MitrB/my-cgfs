#pragma once

#include "object.hpp"
#include "settings.hpp"

#include <vector>

#include <glm/glm.hpp>

/* Coordinate system:
 * The middle of the screen should be (0, 0)
 * X grows positivly to the right
 * Y grows negativly down
 * Z grows positivly in the direction through the screen
 */
namespace scenario {

class Canvas {
  public:
    Canvas(std::vector<int> resolution);
    ~Canvas(){};

    std::vector<int> getResolution() { return RESOLUTION; }

  private:
    std::vector<int> RESOLUTION{640, 640};   // WIDHT HEIGHT
};

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

struct PointLight {
    glm::vec3 position;
    glm::vec3 diffusionIntensity;
    glm::vec3 specularIntensity;
};

class Scene {
  public:
    Scene(Settings settings);
    ~Scene();

    Camera *camera;
    ViewPort viewPort;
    Canvas *canvas;

    // World scene
    glm::vec3 backColor;
    glm::vec3 ambientLight{.0f};
    std::vector<Sphere> spheres{};
    std::vector<PointLight> lights{};
};

}   // namespace scenario