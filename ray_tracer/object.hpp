#pragma once

#include "material.hpp"

#include <glm/glm.hpp>

class Sphere {
  public:
    Sphere(glm::vec3 position, float radius, Material material);
    ~Sphere(){};

    glm::vec3 getPosition() { return this->position; }
    float getRadius() { return this->radius; }

    glm::vec3 getSpecularityConstant() { return this->material.specularConstant; }
    glm::vec3 getDiffuseConstant() {return this->material.diffuseConstant;}
    glm::vec3 getAmbientConstant() {return this->material.ambientConstant;}
    float getShineFactor() {return this->material.shineFactor;}

  private:
    glm::vec3 position;
    float radius;

    Material material;
};
