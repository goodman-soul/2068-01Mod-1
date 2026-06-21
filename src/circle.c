#include "circle.h"
#include <stdio.h>

#define PI 3.14159

// Override Implementations

static void circle_draw_impl(void* self_ptr) {
    Circle* self = (Circle*)self_ptr;
    printf("Circle[派生类] 位于 (%d, %d) 半径为 %d\n", 
           self->base.x, self->base.y, self->radius);
}

static double circle_area_impl(Shape* list_ptr) {
    Circle* self = (Circle*)list_ptr; // Safe downcast
    return PI * self->radius * self->radius;
}

void Circle_Init(Circle* self, int x, int y, int radius) {
    // Call super constructor
    Shape_Init((Shape*)self, x, y);
    
    self->radius = radius;
    
    // Override vtable
    self->base.vtable[SHAPE_DRAW] = circle_draw_impl;
    self->base.vtable[SHAPE_AREA] = (void (*)(void*))circle_area_impl;
}
