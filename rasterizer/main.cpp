#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

#include <glm/glm.hpp>

#include "model.hpp"
#include "tgaimage.hpp"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

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
    glm::vec3 LEFT_DOWN_POS{-.5f, .5f,
                            1.f};   // remember the Y axis grows downwards
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

void
drawLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {

    bool transposed = false;
    if (std::abs(y1 - y0) > std::abs(x1 - x0)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        transposed = true;
    }

    // always draw left to right
    if (x1 < x0) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;
    int yi = 1;
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    int error = (2 * dy) - dx;
    int y = y0;
    if (transposed) {
        for (int x = x0; x < x1; x++) {
            image.set(y, x, color);
            if (error > 0) {
                y += yi;
                error += (2 * (dy - dx));
            } else {
                error += 2 * dy;
            }
        }

    } else {
        for (int x = x0; x < x1; x++) {
            image.set(x, y, color);
            if (error > 0) {
                y += yi;
                error += (2 * (dy - dx));
            } else {
                error += 2 * dy;
            }
        }
    }
}

int
main(int argc, char **argv) {
    // Init

    int width = 1080;
    int height = 1080;

    TGAImage image(width, height, TGAImage::RGB);
    Model model("obj/model.obj");

    for (int i = 0; i < model.nfaces(); i++) {
        std::vector<int> face = model.face(i);
        for (int j = 0; j < 3; j++) {
            glm::vec3 v0 = model.vert(face[j]);
            glm::vec3 v1 = model.vert(face[(j + 1) % 3]);
            int x0 = (v0.x + 1.) * width / 2.;
            // std::cout << v0[0] << '\n';
            int y0 = (v0.y + 1.) * height / 2.;
            int x1 = (v1.x + 1.) * width / 2.;
            int y1 = (v1.y + 1.) * height / 2.;
            drawLine(x0, y0, x1, y1, image, white);
        }
    }

    image.flip_vertically();
    image.write_tga_file("out.tga");
    return 0;

    return 0;
}
