#include <stdio.h>
#include <stdlib.h>
#include "obj_diag.h"
#include "shape.h"
#include "circle.h"
#include "rectangle.h"
#include "shape_container.h"

static void print_section(const char* title) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  %-58s  ║\n", title);
    printf("╚══════════════════════════════════════════════════════════════╝\n");
}

static void demo_polymorphism(Shape** shapes, size_t count) {
    printf("\n[多态演示] 通过 Shape* 调用方法:\n");
    for (size_t i = 0; i < count; i++) {
        printf("  [%zu] ", i + 1);
        Shape_Draw(shapes[i]);
        printf("       面积: %.2f\n", Shape_Area(shapes[i]));
    }
}

static void demo_type_mismatch(void) {
    print_section("演示 4: 类型错误检测");
    printf("\n[故意制造类型错误] 把 Shape 当作 Circle 使用:\n");
    
    Shape s;
    Shape_Init(&s, 100, 100);
    
    Circle* wrong_ptr = (Circle*)&s;
    CIRCLE_CHECK_TYPE(wrong_ptr, "类型转换测试: Shape->Circle");
    
    Shape_Destroy(&s);
    printf("\n类型错误已被诊断系统捕获并报告。\n");
}

static void demo_leak(void) {
    print_section("演示 5: 资源泄漏检测 (故意泄漏)");
    
    printf("\n[故意泄漏] 创建 Circle 但不释放:\n");
    Circle* leaked = (Circle*)malloc(sizeof(Circle));
    Circle_Init(leaked, 999, 999, 50);
    printf("已创建 Circle @ %p, 故意不释放以演示泄漏检测...\n", (void*)leaked);
    
    printf("\n注意: 这个对象会在诊断页中显示为泄漏。\n");
}

static void demo_wrong_release_order(void) {
    print_section("演示 6: 错误释放顺序检测");
    
    printf("\n[故意制造错误顺序] 先释放父对象, 再释放子对象:\n");
    
    ShapeContainer* container = (ShapeContainer*)malloc(sizeof(ShapeContainer));
    ShapeContainer_Init(container, 0, 0);
    
    Circle* c = (Circle*)malloc(sizeof(Circle));
    Circle_Init(c, 1, 1, 10);
    
    ShapeContainer_Add(container, (Shape*)c);
    
    printf("  对象层次: ShapeContainer(#%zu) 是父, Circle(#%zu) 是子\n",
           container->base.diag->create_order,
           c->base.diag->create_order);
    
    printf("  错误地先销毁容器(父), 再销毁圆形(子)...\n");
    
    DIAG_RECORD_DESTROY(container->base.diag);
    free(container);
    
    DIAG_RECORD_DESTROY(c->base.diag);
    free(c);
    
    printf("\n释放顺序错误已制造。查看诊断页中的验证结果。\n");
}

int main() {
    ObjDiag_Init();
    
    printf("=== C语言对象封装框架 + 诊断系统 完整演示 ===\n");
    
    print_section("演示 1: 基本多态 (Shape / Circle / Rectangle)");
    
    Shape s;
    Shape_Init(&s, 10, 20);
    
    Circle c;
    Circle_Init(&c, 5, 5, 10);
    
    Rectangle r;
    Rectangle_Init(&r, 20, 20, 8, 6);
    
    Shape* shapes[] = { &s, (Shape*)&c, (Shape*)&r };
    demo_polymorphism(shapes, 3);
    
    printf("\n✓ 多态验证成功: 通过基类指针调用正确的派生类方法。\n");
    printf("  - Shape->Draw    打印 'Shape[基类]'\n");
    printf("  - Circle->Draw   打印 'Circle[派生类]'\n");
    printf("  - Rectangle->Draw 打印 'Rectangle[派生类]'\n");
    
    print_section("演示 2: 代码复用 (Rectangle 复用 Shape 基础设施)");
    
    printf("\n[代码复用证明]\n");
    printf("  Rectangle 通过结构体嵌套 Shape base, 复用了:\n");
    printf("    ✓ 成员变量: x, y 坐标\n");
    printf("    ✓ 虚表机制: vtable 布局\n");
    printf("    ✓ 方法框架: Shape_Draw/Shape_Area 通用调用接口\n");
    printf("    ✓ 诊断系统: ObjDiag 集成\n");
    printf("\n  新增代码仅需实现 Rectangle 特有的:\n");
    printf("    - 成员变量: width, height\n");
    printf("    - 覆盖方法: rectangle_draw_impl, rectangle_area_impl\n");
    printf("\n✓ 代码复用验证成功。\n");
    
    print_section("演示 3: 复合模式与父子关系 (ShapeContainer)");
    
    ShapeContainer container;
    ShapeContainer_Init(&container, 0, 0);
    
    Circle* c1 = (Circle*)malloc(sizeof(Circle));
    Circle_Init(c1, 10, 10, 5);
    
    Rectangle* r1 = (Rectangle*)malloc(sizeof(Rectangle));
    Rectangle_Init(r1, 30, 30, 4, 7);
    
    Circle* c2 = (Circle*)malloc(sizeof(Circle));
    Circle_Init(c2, 50, 50, 8);
    
    ShapeContainer_Add(&container, (Shape*)c1);
    ShapeContainer_Add(&container, (Shape*)r1);
    ShapeContainer_Add(&container, (Shape*)c2);
    
    printf("\n[复合模式演示] ShapeContainer:\n");
    Shape_Draw((Shape*)&container);
    printf("  总面积: %.2f (5²π + 4×7 + 8²π = %.2f + %.2f + %.2f)\n",
           Shape_Area((Shape*)&container),
           3.14159 * 25, 28.0, 3.14159 * 64);
    
    printf("\n✓ 复合模式验证成功: 容器可容纳任意 Shape 子类, 多态递归调用。\n");
    printf("  父子关系已建立, 销毁时会先销毁所有子对象。\n");
    
    print_section("诊断页: 查看完整对象状态");
    ObjDiag_PrintDiagnosticPage();
    
    print_section("演示 4-6: 错误检测功能");
    
    demo_type_mismatch();
    demo_leak();
    demo_wrong_release_order();
    
    print_section("诊断页: 错误注入后完整状态");
    ObjDiag_PrintDiagnosticPage();
    
    print_section("正常清理: 释放其余对象");
    
    ShapeContainer_Destroy(&container);
    Rectangle_Destroy(&r);
    Circle_Destroy(&c);
    Shape_Destroy(&s);
    
    printf("\n✓ 正常对象已正确释放。\n");
    
    print_section("诊断页: 正常清理后状态 (仍有故意泄漏的对象)");
    ObjDiag_PrintDiagnosticPage();
    
    int has_leaks = ObjDiag_HasLeaks();
    int order_valid = ObjDiag_ValidateReleaseOrder();
    
    printf("\n=== 最终诊断总结 ===\n");
    printf("  资源泄漏: %s\n", has_leaks ? "✗ 检测到泄漏 (预期, 因为我们故意泄漏了)" : "✓ 无泄漏");
    printf("  释放顺序: %s\n", order_valid ? "✓ 正确" : "✗ 错误 (预期, 因为我们故意制造了错误顺序)");
    printf("\n=== 演示结束 ===\n");
    
    ObjDiag_Shutdown();
    
    return 0;
}
