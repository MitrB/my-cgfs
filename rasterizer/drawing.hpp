#pragma once

#include "model.hpp"
#include "objects.hpp"
#include "tgaimage.hpp"
#include <glm/fwd.hpp>
#include <vector>

namespace drawing {

void drawLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void drawLine(const objects::Line line,TGAImage &image ); // this calls the drawline function above

void drawModel(Model &model, TGAImage &image, float* zbuffer);

void drawTriangle(const objects::Triangle &triangle, TGAImage &image, float* zbuffer);

}   // namespace drawing