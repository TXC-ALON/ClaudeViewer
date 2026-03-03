# 4. 测试、基准与文档

## 4.1 测试与性能
- 单元测试：使用 GTest，重点覆盖静态拓扑正确性（如回路形成判定）。
- 基准测试：使用 Google Benchmark，评估大规模静态网表解析速度。
- 长耗时仿真：必须运行在非 UI 线程（无锁计算场景重点关注线程边界）。

## 4.2 Doxygen 注释规范

### 4.2.1 强制注释范围
所有公开/受保护的类、函数、枚举、宏必须使用 Doxygen 注释。

### 4.2.2 注释模板
- 类/结构体：`@brief`、`@author`、`@date`、`@note`（关键设计点）
- 函数：`@brief`、`@param[in/out]`、`@return`、`@throw`、`@note`（线程安全等）
- 模板：`@tparam`（参数约束）

示例：
```cpp
/**
 * @brief 根据ID查找引脚（遵循C++17 optional要求）
 * @param[in] pinId 引脚唯一标识，非空
 * @return std::optional<Pin> 找到则返回引脚对象，未找到则返回std::nullopt
 * @note 线程安全：该函数为只读操作，无需加锁
 * @author xxx
 * @date 2026-03-03
 */
std::optional<Pin> findPinByID(const std::string& pinId);
```

### 4.2.3 注释原则
- 禁止无意义注释，仅解释设计动机（Why），不重复代码语义（What）。
- 复杂逻辑（如 `if constexpr` 分支、无锁计算）必须用块注释说明思路。
- 信号槽关联处需注明业务语义，而非仅描述“连接了 A 和 B”。

## 4.3 文档构建与 CI
- 在 CMake 中集成 Doxygen（`find_package(Doxygen)`）。
- 自动生成 API 文档。
- 文档缺失视为编译警告，需在 CI 中校验。
