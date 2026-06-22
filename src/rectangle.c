#include "rectangle.h"
#include <stdio.h>

static void rectangle_draw_impl(void* self_ptr) {
    Rectangle* self = (Rectangle*)self_ptr;
    printf("Rectangle[派生类] 位于 (%d, %d) 宽=%d 高=%d\n",
           self->base.x, self->base.y, self->width, self->height);
}

static double rectangle_area_impl(Shape* list_ptr) {
    Rectangle* self = (Rectangle*)list_ptr;
    return (double)self->width * (double)self->height;
}

static void rectangle_destroy_impl(void* self_ptr) {
    Rectangle* self = (Rectangle*)self_ptr;
    DIAG_RECORD_DESTROY(self->base.diag);
    printf("Rectangle[派生类] 销毁 @ %p (宽=%d, 高=%d)\n",
           (void*)self, self->width, self->height);
}

void Rectangle_Init(Rectangle* self, int x, int y, int width, int height) {
    Shape_Init((Shape*)self, x, y);
    
    self->width = width;
    self->height = height;
    
    self->base.vtable[SHAPE_DRAW] = rectangle_draw_impl;
    self->base.vtable[SHAPE_AREA] = (void (*)(void*))rectangle_area_impl;
    self->base.vtable[SHAPE_DESTROY] = rectangle_destroy_impl;
    
    self->base.diag->type_id = TYPEID_RECTANGLE;
    self->base.diag->type_name = "Rectangle";
    self->base.diag->vtable_size = 3;
}

void Rectangle_Destroy(Rectangle* self) {
    if (!self) return;
    RECTANGLE_CHECK_TYPE(self, "Rectangle_Destroy");
    
    Shape_Destroy_Fn fn = (Shape_Destroy_Fn)self->base.vtable[SHAPE_DESTROY];
    fn((Shape*)self);
}
