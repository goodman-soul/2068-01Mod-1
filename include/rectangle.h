#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "shape.h"

typedef struct {
    Shape base;
    int width;
    int height;
} Rectangle;

void Rectangle_Init(Rectangle* self, int x, int y, int width, int height);
void Rectangle_Destroy(Rectangle* self);

#define RECTANGLE_CHECK_TYPE(self, context) \
    DIAG_CHECK_TYPE((self) ? (self)->base.diag : NULL, TYPEID_RECTANGLE, (context))

#endif
