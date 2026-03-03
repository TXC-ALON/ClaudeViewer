# 1. 代码风格与版本控制

## 1.1 代码格式化（.clang-format）
项目强制执行统一格式化，`.clang-format` 必须纳入版本控制。

## 1.2 静态检查（Clang-Tidy）
启用 Clang-Tidy，重点规则集：
- `modernize-*`：适配 C++17 特性
- `performance-*`：性能优化（避免不必要拷贝等）
- `cppcoreguidelines-*`：内存安全与规范

执行要求：
- 错误级警告禁止忽略。
- 警告级问题需人工评审并记录原因。

## 1.3 Git 提交规范（Conventional Commits）
提交信息格式：`类型: 描述`

约束：
- 描述长度不超过 50 字符。
- 使用中文描述变更内容。
- 复杂修改需在备注中补充设计思路。

类型：
- `feat`：新功能
- `fix`：修复 Bug
- `chore`：构建/工具链变更
- `refactor`：重构（不改变功能）
- `docs`：文档更新
- `style`：纯格式调整
- `test`：测试相关
- `perf`：性能优化
- `revert`：回滚提交

## 1.4 分支管理规范
- 主分支：`main`（稳定版），禁止直接提交。
- 功能分支：`feature/xxx`（示例：`feature/cpp17-optional`）。
- 修复分支：`bugfix/xxx`（示例：`bugfix/pimpl-compile-speed`）。
- 所有改动必须通过 MR/PR 并完成评审后合并。
