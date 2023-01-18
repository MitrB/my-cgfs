#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>

#include <glm/glm.hpp>

/* Coordinate system:
 * The middle of the screen should be (0, 0)
 * X grows positivly to the right
 * Y grows negativly down
 * Z grows positivly in the direction through the screen
 */
namespace rayTracer {

class Canvas { public: Canvas(){};
        ~Canvas(){};

        std::vector<int> getResolution() {return RESOLUTION;}
    private:
        const std::vector<int> RESOLUTION {1080, 1080}; // WIDHT HEIGHT
};

class ViewPort {
    // Assume the viewport lies in the YZ plane
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
        glm::vec3 position {0.f, 0.f, 0.f};
};

class Sphere {
    public:
        Sphere(glm::vec3 position, float radius, glm::vec3 color);
        ~Sphere(){};

        glm::vec3   getPosition()   {return this->position;}
        float       getRadius()     {return this->radius;}
        glm::vec3   getColor()      {return this->color;}
    private:
        glm::vec3 position;
        float radius;
        glm::vec3 color;
};

Sphere::Sphere(glm::vec3 position, float radius, glm::vec3 color) {
    this->position = position;
    this->radius = radius;
    this->color = color;   
}

struct PointLight {
    glm::vec3 position;
    float intensity;
};

}


/**
 * Returns all the spheres that lie in the line formed by the vector vec
 * @param closest should be initialized as 0
 */
std::vector<rayTracer::Sphere> intersectedSpheres(glm::vec3 direction, std::vector<rayTracer::Sphere> spheres, std::vector<glm::vec3>& intersectedPoints, int &closest) {
    std::vector<rayTracer::Sphere> intersectedSpheres{};

    float closestLength = std::numeric_limits<float>::infinity();
    int iterIndex = 0;
    for (rayTracer::Sphere sphere : spheres) {
        glm::vec3 point = sphere.getPosition();
        glm::vec3 projectedVector = glm::dot(direction, point)/glm::length(direction) * glm::normalize(direction);
        if (glm::distance(point, projectedVector) <= sphere.getRadius()) {

            intersectedSpheres.push_back(sphere);
            
            // calculate point that is closest to camera and on the intersected sphere
            float length = glm::length(projectedVector) - sqrt(pow(sphere.getRadius(), 2) - pow(glm::length(projectedVector - point), 2));
            // Use this length to keep the index of the point that is the overall closest to the camera
            if (length <= closestLength) {
                closest = iterIndex; // the closest point index is the index of the sphere that will now be added to the sphere interesctions
                closestLength = length;
            }
            glm::vec3 intersectPoint = length * glm::normalize(direction);
            intersectedPoints.push_back(intersectPoint);
            iterIndex++;
        } 
    }
    return intersectedSpheres;
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

    // Scene
    rayTracer::Sphere sphere({0.f, -0.5f, 5.0f}, 1.f, {.9f, .5f, .2f});
    rayTracer::Sphere sphere2({0.f, 0.5f, 5.5f}, 1.f, {.2f, .9f, .2f});
    std::vector<rayTracer::Sphere> spheres {sphere, sphere2};

    // Light
    rayTracer::PointLight light{};
    light.position = {0.f, -2.f, 0.f};
    light.intensity = 1.f;
    std::vector<rayTracer::PointLight> lights{light};

    float ambientLight = .2f;

    int width = resolution[0];
    int height = resolution[1];
    // buffer for writing scene to file
    std::vector<std::vector<float>> framebuffer{};

    // Iterate over every pixel
    for (size_t j = 0; j<height; j++) {
        for (size_t i = 0; i<width; i++) {

            // background color
            glm::vec3 color = {1.f, 1.f, 1.f};
            std::vector<glm::vec3> intersectedPoints{};
            int closest = 0;

            std::vector<rayTracer::Sphere> intersected = intersectedSpheres(glm::vec3 {pixelWidth*i - viewPortWidth/2 + pixelWidth/2, pixelHeight*j - viewPortHeight/2 + pixelHeight/2, viewPort.getZ()}, spheres, intersectedPoints, closest);

            if (intersected.size() > 0) {

                // base sphere color
                color = intersected[closest].getColor();

                // calculate light 
                float diffuseLight = 0;
                for (rayTracer::PointLight light : lights) {
                    glm::vec3 lightDir = glm::normalize(light.position - intersectedPoints[closest]); 
                    glm::vec3 normalVector = glm::normalize(intersectedPoints[closest] - intersected[closest].getPosition());
                    diffuseLight += ambientLight + light.intensity * std::max(0.f, glm::dot(normalVector, lightDir));
                }
                color = diffuseLight * color; 
            }

            std::vector<float> colorWritable{};
            for (int i = 0; i < 3; i++) {
                colorWritable.push_back(color[i]);
            }

            framebuffer.push_back(colorWritable);
        }
    }

    render(width, height, framebuffer);

    return 0;
}

