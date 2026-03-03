# Processing 记录

本文件用于记录项目每一阶段做了什么、对应时间和提交信息。
时区默认使用 `UTC+08:00`（Asia/Hong_Kong）。

## 记录规则（后续持续执行）

1. 每完成一个阶段并提交后，必须在本文件追加一条记录。
2. 每条记录至少包含：
   - 阶段编号
   - 起止时间（或提交时间）
   - 主要变更内容
   - 提交哈希与提交信息
3. 同一阶段存在多次提交时，按时间顺序列出多条。

## 阶段记录

### 阶段1: 项目基础架构搭建

- **时间**: 2026-03-03 ~ 2026-03-04
- **主要变更内容**:
  - 创建项目目录结构 (include/, src/)
  - 实现Model层: Obj, Inst, Pin, Net, ObjectManager
  - 实现Parser层: CommandParser, CommandDispatcher
  - 实现Layout层: LayoutInfo, InstLayout, PinLayout, NetLayout, LayoutManager
  - 实现View层: InstGraphicsItem, PinGraphicsItem, SchematicScene
  - 创建CMakeLists.txt构建配置
  - 创建Qt viewer主程序和CLI工具
  - 创建单元测试框架

- **创建的文件**:
  - include/model/: obj.h, inst.h, pin.h, net.h, object_manager.h
  - src/model/: obj.cpp, inst.cpp, pin.cpp, net.cpp, object_manager.cpp
  - include/parser/: command_parser.h, command_dispatcher.h
  - src/parser/: command_parser.cpp, command_dispatcher.cpp
  - include/layout/: layout_info.h, layout_manager.h
  - src/layout/: layout_info.cpp, layout_manager.cpp
  - include/view/: inst_graphics_item.h, pin_graphics_item.h, schematic_scene.h
  - src/view/: inst_graphics_item.cpp, pin_graphics_item.cpp, schematic_scene.cpp
  - tools/: qt_viewer_main.cpp, pipeline_cli.cpp
  - tests/: object_manager_tests.cpp
  - CMakeLists.txt

