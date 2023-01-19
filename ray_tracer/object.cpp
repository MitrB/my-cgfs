
#include "object.hpp"

Sphere::Sphere(glm::vec3 position, float radius, Material material) {
    this->position = position;
    this->radius   = radius;
    this->material = material;
}
