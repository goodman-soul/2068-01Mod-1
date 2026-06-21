#include "shape.h"
#include <stdio.h>

// Default Implementations

static void default_draw_impl(void* self_ptr) {
    Shape* self = (Shape*)self_ptr;
    printf("Shape[基类] 位于 (%d, %d)\n", self->x, self->y);
}

// Note: We return void* or use a wrapper to handle the return type mismatch in a raw void(*)(void*) array,
// but for this implementation we will cast the function pointer at the call site.
// Ths implementation must match the signature expected by the caller after casting.
static double default_area_impl(Shape* self) {
    return 0.0;
}

void Shape_Init(Shape* self, int x, int y) {
    self->x = x;
    self->y = y;
    // Initialize vtable
    self->vtable[SHAPE_DRAW] = default_draw_impl;
    
    // We cast to void(*)(void*) to store in the array.
    // This is safe as long as we cast it back correctly before calling.
    self->vtable[SHAPE_AREA] = (void (*)(void*))default_area_impl;
}

void Shape_Draw(Shape* self) {
    // Determine the function
    Shape_Draw_Fn fn = (Shape_Draw_Fn)self->vtable[SHAPE_DRAW];
    fn(self);
}

double Shape_Area(Shape* self) {
    // Determine the function
    Shape_Area_Fn fn = (Shape_Area_Fn)self->vtable[SHAPE_AREA];
    return fn(self);
}
