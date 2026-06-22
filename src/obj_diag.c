#include "obj_diag.h"
#include "shape.h"
#include "shape_container.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_OBJECTS 256

static ObjDiagInfo* g_registry[MAX_OBJECTS];
static size_t g_registry_count = 0;
static size_t g_create_counter = 0;
static size_t g_destroy_counter = 0;
static int g_initialized = 0;

static size_t g_expected_release_order[MAX_OBJECTS];
static size_t g_expected_release_count = 0;

void ObjDiag_Init(void) {
    if (g_initialized) return;
    memset(g_registry, 0, sizeof(g_registry));
    g_registry_count = 0;
    g_create_counter = 0;
    g_destroy_counter = 0;
    g_expected_release_count = 0;
    g_initialized = 1;
}

void ObjDiag_Shutdown(void) {
    for (size_t i = 0; i < g_registry_count; i++) {
        if (g_registry[i]) {
            if (g_registry[i]->children) {
                free(g_registry[i]->children);
            }
            free(g_registry[i]);
        }
    }
    g_registry_count = 0;
    g_initialized = 0;
}

ObjDiagInfo* ObjDiag_RegisterObject(void* obj_ptr,
                                    ObjTypeID type_id,
                                    const char* type_name,
                                    const char* file,
                                    int line,
                                    void** vtable,
                                    size_t vtable_size,
                                    const char** method_names) {
    if (!g_initialized) ObjDiag_Init();
    if (g_registry_count >= MAX_OBJECTS) return NULL;

    ObjDiagInfo* info = (ObjDiagInfo*)malloc(sizeof(ObjDiagInfo));
    if (!info) return NULL;

    memset(info, 0, sizeof(ObjDiagInfo));
    info->magic = OBJ_MAGIC;
    info->type_id = type_id;
    info->type_name = type_name;
    info->obj_ptr = obj_ptr;
    info->parent = NULL;
    info->children = NULL;
    info->child_count = 0;
    info->child_capacity = 0;
    info->create_order = ++g_create_counter;
    info->destroy_order = 0;
    info->is_destroyed = 0;
    info->create_file = file;
    info->create_line = line;
    info->vtable = vtable;
    info->vtable_size = vtable_size;
    info->method_names = method_names;

    g_registry[g_registry_count++] = info;
    return info;
}

void ObjDiag_SetParent(ObjDiagInfo* child, ObjDiagInfo* parent) {
    if (!child || !parent) return;
    
    child->parent = parent;
    
    if (parent->child_count >= parent->child_capacity) {
        size_t new_cap = parent->child_capacity == 0 ? 4 : parent->child_capacity * 2;
        ObjDiagInfo** new_children = (ObjDiagInfo**)realloc(
            parent->children, new_cap * sizeof(ObjDiagInfo*));
        if (!new_children) return;
        parent->children = new_children;
        parent->child_capacity = new_cap;
    }
    
    parent->children[parent->child_count++] = child;
}

void ObjDiag_UnregisterObject(ObjDiagInfo* info) {
    if (!info) return;
    
    for (size_t i = 0; i < g_registry_count; i++) {
        if (g_registry[i] == info) {
            for (size_t j = i; j < g_registry_count - 1; j++) {
                g_registry[j] = g_registry[j + 1];
            }
            g_registry_count--;
            break;
        }
    }
    
    if (info->children) {
        free(info->children);
    }
    free(info);
}

void ObjDiag_RecordDestroy(ObjDiagInfo* info) {
    if (!info) return;
    info->is_destroyed = 1;
    info->destroy_order = ++g_destroy_counter;
}

int ObjDiag_CheckType(const ObjDiagInfo* info, ObjTypeID expected_type) {
    if (!info) return 0;
    if (info->magic != OBJ_MAGIC) return 0;
    
    if (info->type_id == expected_type) return 1;
    
    if (expected_type == TYPEID_SHAPE) {
        if (info->type_id == TYPEID_CIRCLE ||
            info->type_id == TYPEID_RECTANGLE ||
            info->type_id == TYPEID_SHAPECONTAINER) {
            return 1;
        }
    }
    
    return 0;
}

const char* ObjDiag_GetTypeName(ObjTypeID type_id) {
    switch (type_id) {
        case TYPEID_SHAPE:          return "Shape";
        case TYPEID_CIRCLE:         return "Circle";
        case TYPEID_RECTANGLE:      return "Rectangle";
        case TYPEID_SHAPECONTAINER: return "ShapeContainer";
        default:                    return "Unknown";
    }
}

static void print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
}

void ObjDiag_PrintObjectHierarchy(const ObjDiagInfo* info, int indent) {
    if (!info) return;
    
    print_indent(indent);
    printf("[%s] #%zu @ %p",
           info->type_name,
           info->create_order,
           info->obj_ptr);
    
    if (info->is_destroyed) {
        printf(" (已释放, 释放顺序 #%zu)", info->destroy_order);
    } else {
        printf(" (活跃)");
    }
    printf("\n");
    
    print_indent(indent + 1);
    printf("创建位置: %s:%d\n", info->create_file, info->create_line);
    
    if (info->parent) {
        print_indent(indent + 1);
        printf("父对象: [%s] #%zu @ %p\n",
               info->parent->type_name,
               info->parent->create_order,
               info->parent->obj_ptr);
    }
    
    for (size_t i = 0; i < info->child_count; i++) {
        ObjDiag_PrintObjectHierarchy(info->children[i], indent + 1);
    }
}

void ObjDiag_PrintFunctionPointers(const ObjDiagInfo* info) {
    if (!info || !info->vtable) return;
    
    size_t actual_size = info->vtable_size;
    if (actual_size == 0 || actual_size > SHAPE_VTABLE_SIZE) {
        actual_size = 3;
    }
    
    printf("  虚表 (vtable) @ %p:\n", (void*)info->vtable);
    for (size_t i = 0; i < actual_size; i++) {
        const char* name = info->method_names ? info->method_names[i] : "?";
        if (info->vtable[i]) {
            printf("    [%zu] %-10s -> %p",
                   i, name, (void*)info->vtable[i]);
            if (i == SHAPE_DRAW) {
                if (info->type_id == TYPEID_SHAPE) {
                    printf(" [Shape::default_draw]");
                } else if (info->type_id == TYPEID_CIRCLE) {
                    printf(" [Circle::circle_draw]");
                } else if (info->type_id == TYPEID_RECTANGLE) {
                    printf(" [Rectangle::rectangle_draw]");
                } else if (info->type_id == TYPEID_SHAPECONTAINER) {
                    printf(" [ShapeContainer::container_draw]");
                }
            } else if (i == SHAPE_AREA) {
                if (info->type_id == TYPEID_SHAPE) {
                    printf(" [Shape::default_area]");
                } else if (info->type_id == TYPEID_CIRCLE) {
                    printf(" [Circle::circle_area]");
                } else if (info->type_id == TYPEID_RECTANGLE) {
                    printf(" [Rectangle::rectangle_area]");
                } else if (info->type_id == TYPEID_SHAPECONTAINER) {
                    printf(" [ShapeContainer::container_area]");
                }
            } else if (i == SHAPE_DESTROY) {
                if (info->type_id == TYPEID_SHAPE) {
                    printf(" [Shape::default_destroy]");
                } else if (info->type_id == TYPEID_CIRCLE) {
                    printf(" [Circle::circle_destroy]");
                } else if (info->type_id == TYPEID_RECTANGLE) {
                    printf(" [Rectangle::rectangle_destroy]");
                } else if (info->type_id == TYPEID_SHAPECONTAINER) {
                    printf(" [ShapeContainer::container_destroy]");
                }
            } else if (i == SHAPECONTAINER_ADD) {
                printf(" [ShapeContainer::container_add]");
            }
            printf("\n");
        } else if (name && strcmp(name, "?") != 0) {
            printf("    [%zu] %-10s -> (未设置)\n", i, name);
        }
    }
}

void ObjDiag_PrintReleaseOrder(void) {
    printf("\n=== 释放顺序记录 ===\n");
    
    ObjDiagInfo* sorted[MAX_OBJECTS];
    memcpy(sorted, g_registry, g_registry_count * sizeof(ObjDiagInfo*));
    
    for (size_t i = 0; i < g_registry_count - 1; i++) {
        for (size_t j = 0; j < g_registry_count - i - 1; j++) {
            size_t a = sorted[j]->destroy_order;
            size_t b = sorted[j + 1]->destroy_order;
            if ((a == 0 && b != 0) || (a != 0 && b != 0 && a > b)) {
                ObjDiagInfo* tmp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = tmp;
            }
        }
    }
    
    int has_destroyed = 0;
    for (size_t i = 0; i < g_registry_count; i++) {
        if (sorted[i]->is_destroyed) {
            has_destroyed = 1;
            printf("  #%zu: [%s] @ %p (创建顺序 #%zu)\n",
                   sorted[i]->destroy_order,
                   sorted[i]->type_name,
                   sorted[i]->obj_ptr,
                   sorted[i]->create_order);
        }
    }
    
    if (!has_destroyed) {
        printf("  (暂无已释放对象)\n");
    }
    
    int valid = ObjDiag_ValidateReleaseOrder();
    printf("\n  释放顺序正确性: %s\n", valid ? "✓ 正确" : "✗ 错误");
}

int ObjDiag_ValidateReleaseOrder(void) {
    for (size_t i = 0; i < g_registry_count; i++) {
        ObjDiagInfo* info = g_registry[i];
        if (!info->is_destroyed) continue;
        
        for (size_t j = 0; j < info->child_count; j++) {
            ObjDiagInfo* child = info->children[j];
            if (child->is_destroyed && 
                child->destroy_order > info->destroy_order) {
                return 0;
            }
        }
    }
    return 1;
}

void ObjDiag_PrintLeaks(void) {
    printf("\n=== 资源泄漏检测 ===\n");
    
    int has_leaks = 0;
    for (size_t i = 0; i < g_registry_count; i++) {
        if (!g_registry[i]->is_destroyed) {
            has_leaks = 1;
            printf("  泄漏: [%s] #%zu @ %p, 创建于 %s:%d\n",
                   g_registry[i]->type_name,
                   g_registry[i]->create_order,
                   g_registry[i]->obj_ptr,
                   g_registry[i]->create_file,
                   g_registry[i]->create_line);
        }
    }
    
    if (!has_leaks) {
        printf("  ✓ 无资源泄漏\n");
    }
}

int ObjDiag_HasLeaks(void) {
    for (size_t i = 0; i < g_registry_count; i++) {
        if (!g_registry[i]->is_destroyed) {
            return 1;
        }
    }
    return 0;
}

void ObjDiag_ReportTypeMismatch(const ObjDiagInfo* info,
                                ObjTypeID expected_type,
                                const char* context) {
    if (!info) {
        fprintf(stderr, "\n*** 类型错误: %s - 空对象指针 ***\n",
                context ? context : "未知上下文");
        return;
    }
    
    fprintf(stderr, "\n*** 类型错误: %s ***\n",
            context ? context : "未知上下文");
    fprintf(stderr, "    对象: [%s] #%zu @ %p\n",
            info->type_name,
            info->create_order,
            info->obj_ptr);
    fprintf(stderr, "    创建于: %s:%d\n",
            info->create_file,
            info->create_line);
    fprintf(stderr, "    期望类型: [%s]\n",
            ObjDiag_GetTypeName(expected_type));
    fprintf(stderr, "    实际类型: [%s]\n",
            ObjDiag_GetTypeName(info->type_id));
    
    if (info->magic != OBJ_MAGIC) {
        fprintf(stderr, "    警告: magic number 不匹配 (0x%x != 0x%lx) - 可能已释放或损坏\n",
                info->magic, OBJ_MAGIC);
    }
}

void ObjDiag_PrintDiagnosticPage(void) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║              对象诊断页 (Object Diagnostic Page)              ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    
    printf("\n=== 1. 对象层次结构 ===\n");
    for (size_t i = 0; i < g_registry_count; i++) {
        if (!g_registry[i]->parent) {
            ObjDiag_PrintObjectHierarchy(g_registry[i], 0);
        }
    }
    
    printf("\n=== 2. 函数指针映射 ===\n");
    for (size_t i = 0; i < g_registry_count; i++) {
        ObjDiagInfo* info = g_registry[i];
        printf("\n  [%s] #%zu @ %p:\n",
               info->type_name, info->create_order, info->obj_ptr);
        ObjDiag_PrintFunctionPointers(info);
    }
    
    ObjDiag_PrintReleaseOrder();
    ObjDiag_PrintLeaks();
    
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                    诊断页结束 (End of Page)                   ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}
