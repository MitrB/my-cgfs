#pragma once

#include "material.hpp"

#include <glm/glm.hpp>

class Sphere {
  public:
    Sphere(glm::vec3 position, float radius, material::Material material);
    ~Sphere(){};

    glm::vec3 getPosition() const { return this->position; }
    float getRadius() const { return this->radius; }

    material::Material getMaterial() const {return this->material;}

  private:
    glm::vec3 position;
    float radius;

    material::Material material;
};
