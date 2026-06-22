#include "circle.h"
#include <stdio.h>

#define PI 3.14159

static void circle_draw_impl(void* self_ptr) {
    Circle* self = (Circle*)self_ptr;
    printf("Circle[派生类] 位于 (%d, %d) 半径为 %d\n",
           self->base.x, self->base.y, self->radius);
}

static double circle_area_impl(Shape* list_ptr) {
    Circle* self = (Circle*)list_ptr;
    return PI * self->radius * self->radius;
}

static void circle_destroy_impl(void* self_ptr) {
    Circle* self = (Circle*)self_ptr;
    DIAG_RECORD_DESTROY(self->base.diag);
    printf("Circle[派生类] 销毁 @ %p (半径=%d)\n",
           (void*)self, self->radius);
}

void Circle_Init(Circle* self, int x, int y, int radius) {
    Shape_Init((Shape*)self, x, y);
    
    self->radius = radius;
    
    self->base.vtable[SHAPE_DRAW] = circle_draw_impl;
    self->base.vtable[SHAPE_AREA] = (void (*)(void*))circle_area_impl;
    self->base.vtable[SHAPE_DESTROY] = circle_destroy_impl;
    
    self->base.diag->type_id = TYPEID_CIRCLE;
    self->base.diag->type_name = "Circle";
    self->base.diag->vtable_size = 3;
}

void Circle_Destroy(Circle* self) {
    if (!self) return;
    CIRCLE_CHECK_TYPE(self, "Circle_Destroy");
    
    Shape_Destroy_Fn fn = (Shape_Destroy_Fn)self->base.vtable[SHAPE_DESTROY];
    fn((Shape*)self);
}
