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

class Sphere {
    public:
        Sphere(glm::vec3 position, float radius, glm::vec3 color, float specularity);
        ~Sphere(){};

        glm::vec3   getPosition()   {return this->position;}
        float       getRadius()     {return this->radius;}
        glm::vec3   getColor()      {return this->color;}
        float       getSpecularity(){return this->specularity;}
    private:
        glm::vec3 position;
        float radius;
        glm::vec3 color;
        float specularity;
};

Sphere::Sphere(glm::vec3 position, float radius, glm::vec3 color, float specularity) {
    this->position = position;
    this->radius = radius;
    this->color = color;   
    this->specularity = specularity;   
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
std::vector<rayTracer::Sphere> intersectedSpheres(glm::vec3 origin, glm::vec3 direction, std::vector<rayTracer::Sphere> spheres, std::vector<glm::vec3>& intersectedPoints, int &closest) {
    std::vector<rayTracer::Sphere> intersectedSpheres{};
    // Calculate everything with the origin as 0 0 0 

    float closestLength = std::numeric_limits<float>::infinity();
    int iterIndex = 0;
    for (rayTracer::Sphere sphere : spheres) {
        glm::vec3 point = sphere.getPosition() - origin;
        // check if the sphere is behind the direction: discard
        if (glm::dot(direction, point) <= 0) {
            continue;
        }
        glm::vec3 projectedVector = glm::dot(direction, point)/glm::length(direction) * glm::normalize(direction);
        if (glm::distance(point, projectedVector) <= sphere.getRadius()) {

            intersectedSpheres.push_back(sphere);
            
            // calculate point that is closest to camera and on the intersected sphere
            float length = glm::length(projectedVector) - sqrt(powf(sphere.getRadius(), 2) - powf(glm::length(projectedVector - point), 2));
            // Use this length to keep the index of the point that is the overall closest to the camera
            if (length <= closestLength) {
                closest = iterIndex; // the closest point index is the index of the sphere that will now be added to the sphere interesctions
                closestLength = length;
            }
            glm::vec3 intersectPoint = length * glm::normalize(direction) + origin; // Convert the intersectionpoint to a global position
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
    std::vector<rayTracer::Sphere> spheres{};
    rayTracer::Sphere sphere({0.0f, -0.5f, 5.0f}, 1.f, {.9f, .5f, .2f}, 1.0f);
    rayTracer::Sphere sphere2({-1.0f, 0.5f, 5.5f}, 1.f, {.2f, .9f, .2f}, 0.1f);
    rayTracer::Sphere sphere3({1.5f, 0.0f, 5.8f}, 1.2f, {.2f, .1f, .7f}, 0.5f);
    spheres.push_back(sphere);
    spheres.push_back(sphere2);
    spheres.push_back(sphere3);

    std::vector<rayTracer::PointLight> lights{};
    // Light
    rayTracer::PointLight light{};
    light.position = {0.f, -8.f, 0.f};
    light.intensity = 1.f;
    lights.push_back(light);

    rayTracer::PointLight light2{};
    light2.position = {-5.f, 0.f, 0.f};
    light2.intensity = .2f;
    //lights.push_back(light2);

    float ambientLight = .1f;

    int width = resolution[0];
    int height = resolution[1];
    // buffer for writing scene to file
    std::vector<std::vector<float>> framebuffer{};

    // Iterate over every pixel
    for (size_t j = 0; j<height; j++) {
        for (size_t i = 0; i<width; i++) {

            // background color
            glm::vec3 color = {0.2f, 0.4f, 0.4f};
            std::vector<glm::vec3> intersectedPoints{};
            int closest = 0;

            glm::vec3 direction = glm::vec3 {pixelWidth*i - viewPortWidth/2 + pixelWidth/2, pixelHeight*j - viewPortHeight/2 + pixelHeight/2, viewPort.getZ()}; // this is relative to the camera
            std::vector<rayTracer::Sphere> intersected = intersectedSpheres(camera.getPosition(), direction, spheres, intersectedPoints, closest);

            if (intersected.size() > 0) {

                // base sphere color
                color = intersected[closest].getColor();

                // calculate light 
                float diffuseLight = 0;
                float specularLight = 0;
                for (rayTracer::PointLight light : lights) {
                    // If blocked by another sphere: skip, this is shadow
                    int _c = 0;
                    // This could get buggy as it checks intersections with it's own spere, might be a possibilty of it not intersecting and then not casting appropriate shadows
                    if (intersectedSpheres(intersectedPoints[closest], light.position - intersectedPoints[closest] , spheres, intersectedPoints, _c).size() > 0) {
                       continue; 
                    }
                    
                    glm::vec3 lightDir = glm::normalize(light.position - intersectedPoints[closest]); 
                    glm::vec3 normalVector = glm::normalize(intersectedPoints[closest] - intersected[closest].getPosition());

                    //diffuse reflection
                    diffuseLight += light.intensity * std::max(0.f, glm::dot(normalVector, lightDir));

                    // specular reflection
                    glm::vec3 lightBounceDir = 2 * glm::dot(lightDir, normalVector)* normalVector - lightDir;
                    float specularity = intersected[closest].getSpecularity();
                    specularLight += light.intensity * specularity * std::max(0.f, glm::dot(-(direction + camera.getPosition()), lightBounceDir));
                    
                }
                //color = ambientLight*color + diffuseLight * color + specularLight * color; // can play around with this
                color = ambientLight + diffuseLight * color + specularLight * color; // can play around with this
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

