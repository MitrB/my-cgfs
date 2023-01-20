
#include "object.hpp"

Sphere::Sphere(glm::vec3 p, float r, material::Material m) : position{p}, radius{r}, material{std::move(m)} {}
