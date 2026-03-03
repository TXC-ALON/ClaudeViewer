# 3. C++17 与对象管理准则

## 3.1 C++17 特性应用
1. 查找操作（如 `findPinByID`）必须返回 `std::optional<T>`。
2. 遍历容器优先使用结构化绑定：`for (auto const& [id, element] : container)`。
3. 模板化绘制逻辑使用 `if constexpr`，减少运行时分支开销。

## 3.2 内存与对象管理
- Qt 视图组件：由对象树（Object Tree）管理。
- 非 UI 逻辑对象：必须使用 `std::unique_ptr` 管理所有权。
- 核心类（如 `CircuitEngine`）：必须使用 PIMPL 隔离实现，降低编译耦合。
- 信号槽连接：强制使用 Qt5 成员函数指针语法，禁止 `SIGNAL()/SLOT()` 宏。
- 成员访问控制：优先 `getter/setter`，禁止直接暴露私有/受保护成员（简单 POD 结构体可例外）。
