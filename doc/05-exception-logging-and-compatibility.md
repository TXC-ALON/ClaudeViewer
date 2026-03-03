# 5. 异常、日志与兼容性

## 5.1 异常处理规范
- UI 线程：禁止抛出裸异常；统一捕获并转换为 Qt 消息框提示。
- 非 UI 线程：允许抛异常，但必须在线程入口统一捕获，记录日志并通知主线程。
- 异常类型：优先 `std::runtime_error`；自定义异常须继承 `std::exception` 并实现 `what()`。

## 5.2 日志规范
- 使用 `qDebug` / `qWarning` / `qCritical` 分级日志（非 UI 线程可配合 `spdlog`）。
- 日志至少包含：时间戳、线程 ID、模块名、错误码（如有）。
- 生产环境通过 `QT_NO_DEBUG_OUTPUT` 禁用 `qDebug`。
- 长耗时仿真记录关键步骤耗时，用于基准对比。

## 5.3 兼容性与可移植性
- C++17 特性使用 `__cpp_lib_xxx` 宏检查编译器支持（GCC 8+ / Clang 7+ / MSVC 2019+）。
- Qt 版本锁定目标版本（如 Qt 5），通过 `QT_VERSION_CHECK` 做兼容处理。
- 跨平台路径使用 `QDir::separator()`，线程调度适配 Windows/Linux，避免硬编码平台逻辑。
