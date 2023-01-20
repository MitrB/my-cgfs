
#include "color.hpp"
#include "tgaimage.hpp"
#include <glm/glm.hpp>

namespace objects {

struct Line {
    glm::vec3 p0;
    glm::vec3 p1;

    TGAColor color{white};
};

struct Triangle {
    glm::vec3 p0;
    glm::vec3 p1;
    glm::vec3 p2;

    TGAColor color{white};
};

}   // namespace objects