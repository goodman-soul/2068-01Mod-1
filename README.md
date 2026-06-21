# C语言模拟C++类封装 (C Encapsulation of C++ Classes)

本项目展示了如何利用C语言的 `struct` 和函数指针数组（虚表 `vtable`）来实现 C++ 风格的面向对象编程（OOP）特性：**封装**、**继承** 和 **多态**。

## 核心概念映射 (Core Concept Mapping)

| C++ 概念 | C 语言实现 | 说明 |
| :--- | :--- | :--- |
| **类 (Class)** | `struct` | 使用结构体保存成员变量 |
| **方法 (Method)** | 函数指针 | 将函数指针存储在结构体或虚表中 |
| **构造函数 (Constructor)** | `Init` 函数 | 手动调用的初始化函数 (e.g., `Shape_Init`) |
| **继承 (Inheritance)** | 结构体嵌套 | 派生类包含基类作为第一个成员 (e.g., `struct Circle { Shape base; ... }`) |
| **多态 (Polymorphism)** | `vtable` (虚表) | 函数指针数组，派生类在构造时覆盖基类的函数指针 |
| **`this` 指针** | 显式 `self` 参数 | 所有方法第一个参数显式传递对象指针 |

## 组件说明

- **Shape (基类)**: 定义接口（Draw, Area）和默认行为。
    - 文件: `include/shape.h`, `src/shape.c`
- **Circle (派生类)**: 继承自 `Shape`，重写（Override）了 `Draw` 和 `Area` 方法。
    - 文件: `include/circle.h`, `src/circle.c`
- **Main (测试程序)**: 演示基类和派生类的实例化及多态调用。
    - 文件: `src/main.c`

## 如何运行 (How to Run)

本项目支持使用 Docker Compose 一键运行，环境隔离，无需本地安装 GCC。

### 1. 启动并构建
```bash
docker compose up --build
```
> 这将编译代码并运行演示程序。

### 2. 清理环境
```bash
docker compose down
```

### 3. 本地运行 (可选)
如果本地已安装 GCC:
```bash
mkdir -p build
gcc src/main.c src/shape.c src/circle.c -I include -o build/demo
./build/demo
```

## 验证步骤 (Verification)

测试人员请重点关注控制台输出，验证多态性是否生效。

### 预期输出与解释

运行 `docker compose up --build` 后，你将看到如下输出：

```text
=== C语言实现C++类封装演示 ===

[1] 测试基类 (Shape):
Shape[基类] 位于 (10, 20)
面积: 0.00
```
> **验证点 1**: 基类 `Shape` 使用默认实现。`Draw` 打印 "Shape[基类]"，`Area` 返回 0.00。

```text
[2] 测试派生类 (Circle):
通过 Shape* 调用方法 (多态性检查):
Circle[派生类] 位于 (5, 5) 半径为 10
面积: 314.16 (预期值 ~314.16)
```
> **验证点 2 (关键)**: 这里进行了**多态**调用。
> - 代码中使用的是 `Shape*` (基类指针) 指向 `Circle` 对象。
> - 调用 `Shape_Draw` 时，实际执行的是 `Circle_Draw` (打印 "Circle[派生类]...")。
> - 调用 `Shape_Area` 时，实际执行的是 `Circle_Area` (计算 πr²)。
> - **如果输出显示 "Shape[基类]" 或 面积为 0，则说明多态实现失败。**

## 扩展测试 (For Developers/Testers)

如果你想尝试添加一个新的形状（例如 `Rectangle`）：
1.  复制 `include/circle.h` 为 `include/rectangle.h`。
2.  复制 `src/circle.c` 为 `src/rectangle.c`。
3.  修改结构体，添加 `width`, `height`。
4.  实现 `Rectangle_Init`，并在其中覆盖 `vtable`。
5.  在 `main.c` 中测试它。
