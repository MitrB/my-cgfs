#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

#include <glm/glm.hpp>

#include "color.hpp"
#include "drawing.hpp"
#include "model.hpp"
#include "tgaimage.hpp"



/* Coordinate system:
 * The middle of the screen should be (0, 0)
 * X grows positivly to the right
 * Y grows negativly down
 * Z grows positivly in the direction through the screen
 */
namespace rasterizer {

class ViewPort {
    // The viewport is relative to the camera
    // Assume the center of the viewport is the position of the viewport
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
    // Assume the position of the camera is the point where we are looking from
    // Assume we are looking towards the positive Z
  public:
    Camera(){};
    ~Camera(){};

    glm::vec3 getPosition() { return this->position; }

  private:
    glm::vec3 position{0.0f, 0.0f, 0.0f};
};

}   // namespace rasterizer

int main(int argc, char **argv) {
    // Init

    int width  = 1080;
    int height = 1080;

    TGAImage image(width, height, TGAImage::RGB);
    Model model("obj/model.obj");

    // drawing::drawModel(model, image);

    // objects::Triangle triangle0 = {{300, 60}, {800, 120}, {600, 930}, red};
    // objects::Triangle triangle1 = {{60, 260}, {100, 250}, {120, 237}, green};
    // objects::Triangle triangle2 = {{600, 60}, {200, 858}, {300, 139}, blue};
    // objects::Triangle triangle3 = {{60, 60}, {200, 858}, {300, 139}, white};
    // drawing::drawTriangle(triangle0, image);
    // drawing::drawTriangle(triangle1, image);
    // drawing::drawTriangle(triangle2, image);
    // drawing::drawTriangle(triangle3, image);

    float zbuffer[height*width];
    for (int i=width*height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

    drawing::drawModel(model, image, zbuffer);


    image.flip_vertically();
    image.write_tga_file("out.tga");
    return 0;

    return 0;
}
