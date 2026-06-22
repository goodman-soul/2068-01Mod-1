#ifndef SHAPE_H
#define SHAPE_H

#include "obj_diag.h"

typedef struct Shape Shape;

#define SHAPE_DRAW    0
#define SHAPE_AREA    1
#define SHAPE_DESTROY 2
#define SHAPE_VTABLE_SIZE 16

typedef void (*Shape_Draw_Fn)(Shape*);
typedef double (*Shape_Area_Fn)(Shape*);
typedef void (*Shape_Destroy_Fn)(Shape*);

extern const char* SHAPE_METHOD_NAMES[SHAPE_VTABLE_SIZE];

struct Shape {
    int x;
    int y;
    void (*vtable[SHAPE_VTABLE_SIZE])(void*);
    ObjDiagInfo* diag;
};

void Shape_Init(Shape* self, int x, int y);
void Shape_Destroy(Shape* self);

void Shape_Draw(Shape* self);
double Shape_Area(Shape* self);

#define SHAPE_CHECK_TYPE(self, context) \
    DIAG_CHECK_TYPE((self) ? (self)->diag : NULL, TYPEID_SHAPE, (context))

#endif
