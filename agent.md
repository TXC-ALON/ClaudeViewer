# 项目协作指导入口（Codex）

本文件是项目级 AI 协作入口，详细规范已按主题拆分到 `doc/`。

## 适用范围
- 适用于本仓库所有 C++17 / Qt6(但是要考虑兼容qt5) 相关实现、重构、测试与文档改动。
- 任何自动化修改（含 AI 生成代码）都必须遵循这里引用的规范。

## 文档索引
- 技术方案：`doc/00-SchematicEngine技术方案.md`
- 代码风格与版本控制：`doc/01-code-style-and-version-control.md`
- 架构设计哲学（MVC、静态布局）：`doc/02-architecture-philosophy.md`
- C++17 与对象管理准则：`doc/03-cpp17-and-object-management.md`
- 测试、基准与 Doxygen：`doc/04-testing-benchmark-and-docs.md`
- 异常、日志与兼容性：`doc/05-exception-logging-and-compatibility.md`
- 术语与类设计详解：`doc/06-术语与类设计详解.md`

## 执行优先级
1. 先遵守架构边界，再做功能实现。
2. 为每一个关键类(如Model类和gui类)建立对应的头文件和源文件，不会将他们挤在一起。
3. 代码提交前必须通过格式化、静态检查、测试。
4. 公开接口与关键逻辑必须具备 Doxygen 注释与设计说明。
5. 执行完一个关键的阶段就将当前进度和必要信息写入Processing.md

## 强制配置文件
- `.clang-format`：统一格式化规则，必须纳入版本控制。
- `.clang-tidy`：统一静态检查规则，错误级问题不得忽略。
