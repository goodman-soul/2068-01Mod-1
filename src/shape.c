#include "shape.h"
#include <stdio.h>

const char* SHAPE_METHOD_NAMES[SHAPE_VTABLE_SIZE] = {
    "Draw", "Area", "Destroy"
};

static void default_draw_impl(void* self_ptr) {
    Shape* self = (Shape*)self_ptr;
    printf("Shape[基类] 位于 (%d, %d)\n", self->x, self->y);
}

static double default_area_impl(Shape* self) {
    (void)self;
    return 0.0;
}

static void default_destroy_impl(void* self_ptr) {
    Shape* self = (Shape*)self_ptr;
    DIAG_RECORD_DESTROY(self->diag);
    printf("Shape[基类] 销毁 @ %p\n", (void*)self);
}

void Shape_Init(Shape* self, int x, int y) {
    self->x = x;
    self->y = y;
    
    for (int i = 0; i < SHAPE_VTABLE_SIZE; i++) {
        self->vtable[i] = NULL;
    }
    
    self->vtable[SHAPE_DRAW] = default_draw_impl;
    self->vtable[SHAPE_AREA] = (void (*)(void*))default_area_impl;
    self->vtable[SHAPE_DESTROY] = default_destroy_impl;
    
    self->diag = DIAG_REGISTER(self, TYPEID_SHAPE, "Shape",
                               self->vtable, 3,
                               SHAPE_METHOD_NAMES);
}

void Shape_Destroy(Shape* self) {
    if (!self) return;
    SHAPE_CHECK_TYPE(self, "Shape_Destroy");
    
    Shape_Destroy_Fn fn = (Shape_Destroy_Fn)self->vtable[SHAPE_DESTROY];
    fn(self);
}

void Shape_Draw(Shape* self) {
    if (!self) return;
    SHAPE_CHECK_TYPE(self, "Shape_Draw");
    
    Shape_Draw_Fn fn = (Shape_Draw_Fn)self->vtable[SHAPE_DRAW];
    fn(self);
}

double Shape_Area(Shape* self) {
    if (!self) return 0.0;
    SHAPE_CHECK_TYPE(self, "Shape_Area");
    
    Shape_Area_Fn fn = (Shape_Area_Fn)self->vtable[SHAPE_AREA];
    return fn(self);
}
