#include "shape_container.h"
#include <stdio.h>
#include <stdlib.h>

const char* SHAPECONTAINER_METHOD_NAMES[SHAPECONTAINER_VTABLE_SIZE] = {
    "Draw", "Area", "Destroy", "Add"
};

static void container_draw_impl(void* self_ptr) {
    ShapeContainer* self = (ShapeContainer*)self_ptr;
    printf("ShapeContainer[复合] 位于 (%d, %d) 包含 %zu 个形状:\n",
           self->base.x, self->base.y, self->count);
    
    for (size_t i = 0; i < self->count; i++) {
        printf("  [%zu] ", i + 1);
        Shape_Draw(self->shapes[i]);
    }
}

static double container_area_impl(Shape* list_ptr) {
    ShapeContainer* self = (ShapeContainer*)list_ptr;
    double total = 0.0;
    
    for (size_t i = 0; i < self->count; i++) {
        total += Shape_Area(self->shapes[i]);
    }
    
    return total;
}

static void container_destroy_impl(void* self_ptr) {
    ShapeContainer* self = (ShapeContainer*)self_ptr;
    
    printf("ShapeContainer[复合] 销毁 @ %p, 先销毁 %zu 个子对象...\n",
           (void*)self, self->count);
    
    for (size_t i = 0; i < self->count; i++) {
        Shape_Destroy(self->shapes[i]);
    }
    
    free(self->shapes);
    DIAG_RECORD_DESTROY(self->base.diag);
    printf("ShapeContainer[复合] 完成销毁 @ %p\n", (void*)self);
}

static void container_add_impl(struct ShapeContainer* self, Shape* shape) {
    if (self->count >= self->capacity) {
        size_t new_cap = self->capacity == 0 ? 4 : self->capacity * 2;
        Shape** new_shapes = (Shape**)realloc(
            self->shapes, new_cap * sizeof(Shape*));
        if (!new_shapes) return;
        self->shapes = new_shapes;
        self->capacity = new_cap;
    }
    
    self->shapes[self->count++] = shape;
    
    DIAG_SET_PARENT(shape->diag, self->base.diag);
}

void ShapeContainer_Init(ShapeContainer* self, int x, int y) {
    Shape_Init((Shape*)self, x, y);
    
    self->shapes = NULL;
    self->count = 0;
    self->capacity = 0;
    
    self->base.vtable[SHAPE_DRAW] = container_draw_impl;
    self->base.vtable[SHAPE_AREA] = (void (*)(void*))container_area_impl;
    self->base.vtable[SHAPE_DESTROY] = container_destroy_impl;
    self->base.vtable[SHAPECONTAINER_ADD] = (void (*)(void*))container_add_impl;
    
    self->base.diag->type_id = TYPEID_SHAPECONTAINER;
    self->base.diag->type_name = "ShapeContainer";
    self->base.diag->vtable_size = SHAPECONTAINER_VTABLE_SIZE;
    self->base.diag->method_names = SHAPECONTAINER_METHOD_NAMES;
}

void ShapeContainer_Destroy(ShapeContainer* self) {
    if (!self) return;
    SHAPECONTAINER_CHECK_TYPE(self, "ShapeContainer_Destroy");
    
    Shape_Destroy_Fn fn = (Shape_Destroy_Fn)self->base.vtable[SHAPE_DESTROY];
    fn((Shape*)self);
}

void ShapeContainer_Add(ShapeContainer* self, Shape* shape) {
    if (!self || !shape) return;
    SHAPECONTAINER_CHECK_TYPE(self, "ShapeContainer_Add");
    SHAPE_CHECK_TYPE(shape, "ShapeContainer_Add(shape参数)");
    
    ShapeContainer_Add_Fn fn = (ShapeContainer_Add_Fn)self->base.vtable[SHAPECONTAINER_ADD];
    fn(self, shape);
}
