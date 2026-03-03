# 2. 核心架构设计哲学

## 2.1 逻辑与表现彻底解耦（MVC）

### Logic Layer（纯 C++17）
- 严禁包含 `QtWidgets` 或 `QGraphicsItem` 相关头文件。
- 允许使用 `QtCore/QtGui` 基础类型（如 `QPointF`、`QUuid`）表达静态坐标。
- 核心职责：
  - 静态拓扑维护
  - 网表生成
  - 物理规则检查（DRC）

### Graphics Layer（Qt GraphicsView）
- `QGraphicsItem` 仅作为 Logic 层数据的视觉映射。
- 图元不承担位移交互逻辑（静态渲染模型）。

## 2.2 静态布局设计
- Model 对象采用 `Obj` 基类 + 派生类（如 `Instance`、`Port`、`Pin`、`Net`）等建模。
- `ObjectManager` 使用多态存储与类型索引管理对象。
- 组合优先用于算法/策略模块；模型对象体系以继承结构保持语义清晰与稳定扩展。
