#ifndef SHAPE_CONTAINER_H
#define SHAPE_CONTAINER_H

#include "shape.h"

typedef struct ShapeContainer ShapeContainer;

#define SHAPECONTAINER_ADD 3
#define SHAPECONTAINER_VTABLE_SIZE (SHAPE_VTABLE_SIZE + 1)

typedef void (*ShapeContainer_Add_Fn)(ShapeContainer*, Shape*);

extern const char* SHAPECONTAINER_METHOD_NAMES[SHAPECONTAINER_VTABLE_SIZE];

typedef struct ShapeContainer {
    Shape base;
    Shape** shapes;
    size_t count;
    size_t capacity;
} ShapeContainer;

void ShapeContainer_Init(ShapeContainer* self, int x, int y);
void ShapeContainer_Destroy(ShapeContainer* self);

void ShapeContainer_Add(ShapeContainer* self, Shape* shape);

#define SHAPECONTAINER_CHECK_TYPE(self, context) \
    DIAG_CHECK_TYPE((self) ? (self)->base.diag : NULL, TYPEID_SHAPECONTAINER, (context))

#endif
