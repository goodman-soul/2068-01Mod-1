#ifndef SHAPE_H
#define SHAPE_H

typedef struct Shape Shape;

// Method Indices in VTable
#define SHAPE_DRAW 0
#define SHAPE_AREA 1
#define SHAPE_VTABLE_SIZE 2

// Function pointer types for safety casting
typedef void (*Shape_Draw_Fn)(Shape*);
typedef double (*Shape_Area_Fn)(Shape*);

struct Shape {
    int x;
    int y;
    // Vtable: Array of generic function pointers
    void (*vtable[SHAPE_VTABLE_SIZE])(void*);
};

// Constructor
void Shape_Init(Shape* self, int x, int y);

// Polymorphic Methods
void Shape_Draw(Shape* self);
double Shape_Area(Shape* self);

#endif
