#ifndef CIRCLE_H
#define CIRCLE_H

#include "shape.h"

typedef struct {
    Shape base; // Inherit Shape
    int radius;
} Circle;

void Circle_Init(Circle* self, int x, int y, int radius);

#endif
