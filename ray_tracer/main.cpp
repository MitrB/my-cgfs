#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

/* Coordinate system:
 * The middle of the screen should be (0, 0)
 * X grows positivly to the right
 * Y grows negativly down
 * Z grows positivly in the direction through the screen
 */

namespace rayTracer {

class Canvas {
    public:
        Canvas(){};
        ~Canvas(){};

        std::vector<int> getResolution() {return RESOLUTION;}
    private:
        const std::vector<int> RESOLUTION {600, 600}; // WIDHT HEIGHT
};

class ViewPort {
    // Assume the viewport lies in the YZ plane
    // Assume the center of the viewport is the position of the viewport
    public:
        ViewPort(){};
        ~ViewPort(){};

        float getZ() {return this->LEFT_DOWN_POS[2];}
        std::vector<float> getLDP() {return LEFT_DOWN_POS;}
        std::vector<float> getRUP() {return RIGHT_UP_POS;}

    private:
        /* +--------RUP
         * |         |
         * LDP-------+
         */
        std::vector<float> LEFT_DOWN_POS {-.5f, .5f, 1.f}; // remember the Y axis grows downwards
        std::vector<float> RIGHT_UP_POS {.5f, -.5f, 1.f};
};


class Camera {
    // Assume the position of the camera is the point where we are looking from
    // Assume we are looking towards the positive Z
    public:
        Camera(){};
        ~Camera(){};

        std::vector<float> getPosition() {return this->position;}
    private:
        std::vector<float> position {0.f, 0.f, 0.f};
};

class Sphere {
    public:
        Sphere(){};
        ~Sphere(){};

        std::vector<float>  getPosition()   {return this->position;}
        float   getRadius()     {return this->radius;}
        std::vector<float>    getColor()      {return this->color;}
    private:
        std::vector<float> position {0.f, 0.f, 10.f};
        float radius {0.01f};
        std::vector<float> color {1.f, 0.f ,0.f};
};

}

namespace utils {

float dot(std::vector<float> vec1, std::vector<float> vec2) {
    float product = 0;
    for (int i = 0; i < vec1.size(); i++) {
       product += vec1[i]* vec2[i];
    } 

    return product;
}

float len(std::vector<float> vec) {
    float length = 0;
    for (float point : vec) {
        length += point*point;
    }

    return std::sqrt(length);
}

std::vector<float> scaleVector(float scalar, std::vector<float> vec) {
    for (int i = 0; i < vec.size(); i++) {
        vec[i] = vec[i]*scalar;
    }

    return vec;
}

float distanceBetweenVectors(std::vector<float> vec1, std::vector<float> vec2) {
    float distance = 0;
    for (int i = 0; i < vec1.size(); i++) {
        distance += vec1[i]*vec1[i] - vec2[i]*vec2[i];
    }
    
    return std::sqrt(distance);
}

// only works if the point's axis origin is the same origin as the vector has
float distancePointVector(std::vector<float> point, std::vector<float> vec) {
    std::vector<float> projectedVector = scaleVector((dot(vec, point)/len(vec)), vec);
    return distanceBetweenVectors(projectedVector, point);
}

std::vector<rayTracer::Sphere> intersectedSpheres(std::vector<float> vec, std::vector<rayTracer::Sphere> spheres) {
    std::vector<rayTracer::Sphere> intersectedSpheres{};
    for (rayTracer::Sphere sphere : spheres) {
        if (distancePointVector(sphere.getPosition(), vec) <= sphere.getRadius()) {
            intersectedSpheres.push_back(sphere);
        } 
    }
    return intersectedSpheres;
}


}

void render(int width, int height, std::vector<std::vector<float>> &framebuffer) {

    std::ofstream ofs; // save the framebuffer to file
    ofs.open("./out.ppm");
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (size_t i = 0; i < height*width; ++i) {
        for (size_t j = 0; j<3; j++) {
            ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
        }
    }
    ofs.close();
}

int main() {
    // Init
    rayTracer::Camera camera{};
    rayTracer::ViewPort viewPort{}; 
    rayTracer::Canvas canvas{};


    std::vector<int> resolution = canvas.getResolution();
    float viewPortWidth = viewPort.getRUP()[0] - viewPort.getLDP()[0];
    float viewPortHeight = - viewPort.getRUP()[1] + viewPort.getLDP()[1];
    std::cout << "Canvas Width: " << resolution[0] << '\n';
    std::cout << "Canvas Height: " <<  resolution[1] << '\n';
    std::cout << "Viewport Width: " <<  viewPortWidth << '\n';
    std::cout << "Viewport Height: " <<  viewPortHeight<< '\n';

    float pixelWidth = viewPortWidth/resolution[0];
    float pixelHeight = viewPortHeight/resolution[1];
    std::cout << "Pixel Width: " <<  pixelWidth  << '\n';
    std::cout << "Pixel Height: " <<  pixelHeight << '\n';

    rayTracer::Sphere sphere{};
    std::vector<rayTracer::Sphere> spheres {sphere};


    int width = resolution[0];
    int height = resolution[1];
    std::vector<std::vector<float>> framebuffer{};

    for (size_t j = 0; j<height; j++) {
        for (size_t i = 0; i<width; i++) {
            std::vector<float> color = {0.f, 0.f, 0.f};
            //std::cout << pixelWidth*width - viewPortWidth/2;
            std::vector<rayTracer::Sphere> intersected = utils::intersectedSpheres(std::vector<float> {pixelWidth*i - viewPortWidth/2, pixelHeight*j - viewPortHeight/2, viewPort.getZ()}, spheres);
            if (intersected.size() > 0) {
                color = intersected[0].getColor();
            }

            framebuffer.push_back(color);
        }
    }

    render(width, height, framebuffer);

    return 0;
}
