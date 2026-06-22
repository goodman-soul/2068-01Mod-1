#ifndef CIRCLE_H
#define CIRCLE_H

#include "shape.h"

typedef struct {
    Shape base;
    int radius;
} Circle;

void Circle_Init(Circle* self, int x, int y, int radius);
void Circle_Destroy(Circle* self);

#define CIRCLE_CHECK_TYPE(self, context) \
    DIAG_CHECK_TYPE((self) ? (self)->base.diag : NULL, TYPEID_CIRCLE, (context))

#endif
