#include "drawing.hpp"
#include "color.hpp"
#include "tgaimage.hpp"

#include <cmath>
#include <glm/glm.hpp>
#include <iostream>

// this could be optimized by making sure the triangles provides are clockwise or cclw
float Sign(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3) { return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y); }
bool pointInTriangle(glm::vec2 pt, glm::vec2 v1, glm::vec2 v2, glm::vec2 v3) {
    bool b1, b2, b3;
    b1 = Sign(pt, v1, v2) < 0.0f;
    b2 = Sign(pt, v2, v3) < 0.0f;
    b3 = Sign(pt, v3, v1) < 0.0f;
    return ((b1 == b2) && (b2 == b3));
}

// Compute barycentric coordinates (u, v, w) for
// point p with respect to triangle (a, b, c)
void Barycentric(glm::vec2 p, glm::vec2 a, glm::vec2 b, glm::vec2 c, float &u, float &v, float &w) {
    glm::vec2 v0 = b - a, v1 = c - a, v2 = p - a;
    float d00   = glm::dot(v0, v0);
    float d01   = glm::dot(v0, v1);
    float d11   = glm::dot(v1, v1);
    float d20   = glm::dot(v2, v0);
    float d21   = glm::dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    v           = (d11 * d20 - d01 * d21) / denom;
    w           = (d00 * d21 - d01 * d20) / denom;
    u           = 1.0f - v - w;
}



namespace drawing {

void drawLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {

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
    int y     = y0;
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

void drawLine(const objects::Line line, TGAImage &image) { drawLine(line.p0[0], line.p0[1], line.p1[0], line.p1[1], image, line.color); }

void drawModelWireFrame(Model &model, TGAImage &image) {
    const int width  = image.get_width();
    const int height = image.get_height();
    for (int i = 0; i < model.nfaces(); i++) {
        const std::vector<int> face = model.face(i);
        for (int j = 0; j < 3; j++) {
            const glm::vec3 v0 = model.vert(face[j]);
            const glm::vec3 v1 = model.vert(face[(j + 1) % 3]);
            const int x0       = (v0.x + 1.) * width / 2.;
            // std::cout << v0[0] << '\n';
            const int y0 = (v0.y + 1.) * height / 2.;
            const int x1 = (v1.x + 1.) * width / 2.;
            const int y1 = (v1.y + 1.) * height / 2.;
            drawLine(x0, y0, x1, y1, image, white);
        }
    }
}

TGAColor randomColor(float intensity) { return TGAColor(255 * intensity, 255 * intensity, 255 * intensity, 255); }

glm::vec3 light_dir{0.f, 0.f, -1.f};   // define light_dir
void drawModel(Model &model, TGAImage &image, float *zbuffer) {
    const int width  = image.get_width();
    const int height = image.get_height();
    for (int i = 0; i < model.nfaces(); i++) {
        std::vector<int> face = model.face(i);
        glm::vec3 screen_coords[3];
        glm::vec3 world_coords[3];
        for (int j = 0; j < 3; j++) {
            glm::vec3 v      = model.vert(face[j]);
            screen_coords[j] = glm::vec3{(v.x + 1.) * width / 2., (v.y + 1.) * height / 2., v.z};
            world_coords[j]  = v;
        }

        // calculate light based on the world
        glm::vec3 n     = glm::cross((world_coords[2] - world_coords[0]), (world_coords[1] - world_coords[0]));
        float intensity = glm::dot(glm::normalize(n), light_dir);
        if (intensity < 0) {
            continue;
        }

        const objects::Triangle triangle{screen_coords[0], screen_coords[1], screen_coords[2], randomColor(intensity)};
        drawTriangle(triangle, image, zbuffer);
    }
}

void drawTriangle(const objects::Triangle &triangle, TGAImage &image, float *zbuffer) {
    // drawLine({triangle.p0, triangle.p1, triangle.color}, image);
    // drawLine({triangle.p1, triangle.p2, triangle.color}, image);
    // drawLine({triangle.p2, triangle.p0, triangle.color}, image);

    int width  = image.get_width();
    int height = image.get_height();

    glm::vec2 boxMin{width, height};
    glm::vec2 boxMax{0, 0};

    boxMin.x = std::min(boxMin.x, triangle.p0.x);
    boxMin.x = std::min(boxMin.x, triangle.p1.x);
    boxMin.x = std::min(boxMin.x, triangle.p2.x);

    boxMin.y = std::min(boxMin.y, triangle.p0.y);
    boxMin.y = std::min(boxMin.y, triangle.p1.y);
    boxMin.y = std::min(boxMin.y, triangle.p2.y);

    boxMax.x = std::max(boxMax.x, triangle.p0.x);
    boxMax.x = std::max(boxMax.x, triangle.p1.x);
    boxMax.x = std::max(boxMax.x, triangle.p2.x);

    boxMax.y = std::max(boxMax.y, triangle.p0.y);
    boxMax.y = std::max(boxMax.y, triangle.p1.y);
    boxMax.y = std::max(boxMax.y, triangle.p2.y);

    // off by one could be a thing here
    for (int i = boxMin.x; i < boxMax.x; i++) {
        for (int j = boxMin.y; j < boxMax.y; j++) {
            float u;
            float v;
            float w;
            Barycentric({i, j}, triangle.p0, triangle.p1, triangle.p2, u, v, w);
            if (u < 0 || v < 0 || w < 0)
                continue;
            glm::vec3 zVal = triangle.p0 * u + triangle.p1 * v + triangle.p2 * w;
            if (zbuffer[int(i + j * width)] < zVal[2]) {
                zbuffer[int(i + j * width)] = zVal[2];
                image.set(i, j, triangle.color);
            }
        }
    }
}

}   // namespace drawing