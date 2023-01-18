#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>

#include <glm/glm.hpp>

#include "tgaimage.hpp"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

/* Coordinate system:
 * The middle of the screen should be (0, 0)
 * X grows positivly to the right
 * Y grows negativly down
 * Z grows positivly in the direction through the screen
 */
namespace rasterizer {

class Canvas { public: Canvas(){};
        ~Canvas(){};

        std::vector<int> getResolution() {return RESOLUTION;}
    private:
        const std::vector<int> RESOLUTION {1080, 1080}; // WIDHT HEIGHT
};

class ViewPort {
    // The viewport is relative to the camera
    // Assume the center of the viewport is the position of the viewport
    public:
        ViewPort(){};
        ~ViewPort(){};

        float getZ() {return this->LEFT_DOWN_POS[2];}
        glm::vec3 getLDP() {return LEFT_DOWN_POS;}
        glm::vec3 getRUP() {return RIGHT_UP_POS;}

    private:
        /* +--------RUP
         * |         |
         * LDP-------+
         */
        glm::vec3 LEFT_DOWN_POS {-.5f, .5f, 1.f}; // remember the Y axis grows downwards
        glm::vec3 RIGHT_UP_POS {.5f, -.5f, 1.f};
};


class Camera {
    // Assume the position of the camera is the point where we are looking from
    // Assume we are looking towards the positive Z
    public:
        Camera(){};
        ~Camera(){};

        glm::vec3 getPosition() {return this->position;}
    private:
        glm::vec3 position {0.0f, 0.0f, 0.0f};
};


}

int main(int argc, char** argv) {
    // Init
    rasterizer::Camera camera{};
    rasterizer::ViewPort viewPort{}; 
    rasterizer::Canvas canvas{};

    TGAImage image(100, 100, TGAImage::RGB);
	image.set(52, 41, red);
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("out.tga");
	return 0;

    return 0;
}

