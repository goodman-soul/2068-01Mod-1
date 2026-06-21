#include <stdio.h>
#include "shape.h"
#include "circle.h"

int main() {
    printf("=== C语言实现C++类封装演示 ===\n\n");

    // 1. 直接实例化基类
    printf("[1] 测试基类 (Shape):\n");
    Shape s;
    Shape_Init(&s, 10, 20);
    // 多态调用
    Shape_Draw(&s);
    printf("面积: %.2f\n\n", Shape_Area(&s));

    // 2. 实例化派生类
    printf("[2] 测试派生类 (Circle):\n");
    Circle c;
    Circle_Init(&c, 5, 5, 10);
    
    // 向上转型为基类指针以演示多态
    Shape* poly_s = (Shape*)&c;
    
    // 通过基类指针调用方法
    printf("通过 Shape* 调用方法 (多态性检查):\n");
    Shape_Draw(poly_s); // 应该调用 Circle 的 draw
    printf("面积: %.2f (预期值 ~314.16)\n", Shape_Area(poly_s)); // 应该调用 Circle 的 area

    return 0;
}
