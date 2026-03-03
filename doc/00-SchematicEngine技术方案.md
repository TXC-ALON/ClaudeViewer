# SchematicViewer 技术方案

## 一、系统架构

```
Commands (JSON) -> CommandParser -> ObjectManager -> LayoutManager -> GraphicsScene
```

### 分层设计

| 层级 | 职责 | 依赖 |
|------|------|------|
| Commands | JSON格式命令字符串 | 无 |
| CommandParser | 解析命令，分发到对应管理器 | nlohmann/json |
| ObjectManager | 管理 Inst/Pin/Net 逻辑对象 | 无 |
| LayoutManager | 管理布局几何信息 | QtCore |
| GraphicsScene | Qt图形渲染 | QtWidgets |

---

## 二、命令协议

### 2.1 命令格式

统一使用 JSON 格式：

```json
{
  "action": "<load|unload|attribute|mark>",
  "type": "<inst|pin|net>",
  "id": "<ObjectID>",
  ... // 类型特定字段
}
```

### 2.2 命令分类

#### Load (加载对象)

**Inst:**
```json
{
  "action": "load",
  "type": "inst",
  "id": "0000000",
  "name": "pic0",
  "module_name": "top",
  "parent_id": "0000000"
}
```

**Pin:**
```json
{
  "action": "load",
  "type": "pin",
  "id": "0000001001",
  "name": "input",
  "parent_id": "0000000",
  "direction": "input"
}
```

**Net:**
```json
{
  "action": "load",
  "type": "net",
  "id": "0000010001",
  "name": "clk"
}
```

#### Unload (删除对象)

```json
{
  "action": "unload",
  "type": "inst",
  "id": "0000001"
}
```

#### Attribute (属性修改)

```json
{
  "action": "attribute",
  "type": "inst",
  "id": "0000001",
  "module_name": "buf"
}
```

```json
{
  "action": "attribute",
  "type": "pin",
  "id": "0000001001",
  "connectionnum": 2,
  "hier_pin_id": "0000002001"
}
```

#### Mark (标记/高亮)

```json
{
  "action": "mark",
  "type": "pin",
  "id": "0000001002",
  "color": "red"
}
```

---

## 三、数据结构

### 3.1 Object层 (Logic)

```cpp
// 对象ID类型
using ObjectID = uint64_t;

// 对象类型枚举
enum class ObjType { INST, PIN, NET };

// Pin方向：input在inst左边，output在inst右边
enum class PinDirection { INPUT, OUTPUT };

// 对象基类
class Obj {
public:
    virtual ~Obj() = default;
    virtual ObjType getType() const = 0;
    ObjectID getId() const { return id_; }

protected:
    ObjectID id_;
};

// Inst - 电路实例
class Inst : public Obj {
public:
    ObjType getType() const override { return ObjType::INST; }

    void setName(std::string name) { name_ = std::move(name); }
    void setModuleName(std::string name) { moduleName_ = std::move(name); }
    void setParentId(ObjectID id) { parentId_ = id; }

    const std::string& getName() const { return name_; }
    const std::string& getModuleName() const { return moduleName_; }
    ObjectID getParentId() const { return parentId_; }
    const std::vector<ObjectID>& getPins() const { return pins_; }

    void addPin(ObjectID pinId) { pins_.push_back(pinId); }
    void removePin(ObjectID pinId);

private:
    std::string name_;
    std::string moduleName_;
    ObjectID parentId_ = 0;  // 0表示TOP
    std::vector<ObjectID> pins_;
};

// Pin - 电路引脚
class Pin : public Obj {
public:
    ObjType getType() const override { return ObjType::PIN; }

    void setName(std::string name) { name_ = std::move(name); }
    void setParentInstId(ObjectID id) { parentInstId_ = id; }
    void setDirection(PinDirection dir) { direction_ = dir; }
    void setConnectedNetId(std::optional<ObjectID> id) { connectedNetId_ = id; }
    void setHierPinId(std::optional<ObjectID> id) { hierPinId_ = id; }
    void setConnectionCount(int count) { connectionCount_ = count; }

    const std::string& getName() const { return name_; }
    ObjectID getParentInstId() const { return parentInstId_; }
    PinDirection getDirection() const { return direction_; }
    std::optional<ObjectID> getConnectedNetId() const { return connectedNetId_; }
    std::optional<ObjectID> getHierPinId() const { return hierPinId_; }
    int getConnectionCount() const { return connectionCount_; }

private:
    std::string name_;
    ObjectID parentInstId_ = 0;
    PinDirection direction_ = PinDirection::INPUT;  // 默认INPUT
    std::optional<ObjectID> connectedNetId_;
    std::optional<ObjectID> hierPinId_;   // 关联的层次Pin ID（可选）
    int connectionCount_ = 0;
};

// Net - 电路网线
class Net : public Obj {
public:
    ObjType getType() const override { return ObjType::NET; }

    void setName(std::string name) { name_ = std::move(name); }

    const std::string& getName() const { return name_; }
    const std::vector<ObjectID>& getConnectedPins() const { return connectedPins_; }

    void addPin(ObjectID pinId) { connectedPins_.push_back(pinId); }
    void removePin(ObjectID pinId);

private:
    std::string name_;
    std::vector<ObjectID> connectedPins_;
};
```

### 3.2 LayoutInfo层 (Geometry)

```cpp
// Pin所在边界：LEFT对应input，RIGHT对应output
enum class PinSide { TOP, BOTTOM, LEFT, RIGHT };

// 布局信息基类
class LayoutInfo {
public:
    virtual ~LayoutInfo() = default;
    ObjectID getObjectId() const { return objectId_; }

protected:
    ObjectID objectId_;
};

// Inst布局信息
class InstLayout : public LayoutInfo {
public:
    void setBoundingBox(const QRectF& rect) { boundingBox_ = rect; }
    void setPosition(const QPointF& pos) { position_ = pos; }

    QRectF getBoundingBox() const { return boundingBox_; }
    QPointF getPosition() const { return position_; }

private:
    QRectF boundingBox_;  // (x, y, width, height)
    QPointF position_;   // 中心点
};

// Pin布局信息
class PinLayout : public LayoutInfo {
public:
    void setAnchorPos(const QPointF& pos) { anchorPos_ = pos; }
    void setSide(PinSide side) { side_ = side; }
    void setOffset(qreal offset) { offset_ = offset; }

    QPointF getAnchorPos() const { return anchorPos_; }
    PinSide getSide() const { return side_; }
    qreal getOffset() const { return offset_; }

private:
    QPointF anchorPos_;   // 在Inst边界上的锚点位置
    PinSide side_;        // 位于哪一侧
    qreal offset_;       // 在该侧的偏移距离
};

// Net布局信息
class NetLayout : public LayoutInfo {
public:
    void setPathPoints(std::vector<QPointF> points) { pathPoints_ = std::move(points); }
    void addPathPoint(const QPointF& point) { pathPoints_.push_back(point); }
    void clearPathPoints() { pathPoints_.clear(); }

    const std::vector<QPointF>& getPathPoints() const { return pathPoints_; }

private:
    std::vector<QPointF> pathPoints_;  // 路径点序列（弯折点）
};
```

---

## 四、核心模块设计

### 4.1 ObjectManager

```cpp
class ObjectManager {
public:
    // 对象查询 - 返回std::optional
    std::optional<std::shared_ptr<Inst>> findInst(ObjectID id);
    std::optional<std::shared_ptr<Pin>> findPin(ObjectID id);
    std::optional<std::shared_ptr<Net>> findNet(ObjectID id);

    // 对象操作
    std::shared_ptr<Inst> createInst(ObjectID id, std::string name);
    std::shared_ptr<Pin> createPin(ObjectID id, std::string name);
    std::shared_ptr<Net> createNet(ObjectID id, std::string name);

    void removeInst(ObjectID id);
    void removePin(ObjectID id);
    void removeNet(ObjectID id);

    // 关系查询
    std::vector<std::shared_ptr<Pin>> getPinsByInst(ObjectID instId);
    std::vector<std::shared_ptr<Pin>> getPinsByNet(ObjectID netId);

private:
    std::unordered_map<ObjectID, std::shared_ptr<Inst>> insts_;
    std::unordered_map<ObjectID, std::shared_ptr<Pin>> pins_;
    std::unordered_map<ObjectID, std::shared_ptr<Net>> nets_;
};
```

### 4.2 CommandParser

```cpp
#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct ParsedCommand {
    std::string action;      // load, unload, attribute, mark
    std::string type;        // inst, pin, net
    ObjectID id;
    json data;              // 剩余参数
};

class CommandParser {
public:
    std::optional<ParsedCommand> parse(const std::string& jsonStr);

private:
    bool validate(const json& j);
    std::optional<ParsedCommand> buildCommand(const json& j);
};

class CommandDispatcher {
public:
    CommandDispatcher(ObjectManager& objMgr);

    void dispatch(const ParsedCommand& cmd);
    void dispatchLoad(const ParsedCommand& cmd);
    void dispatchUnload(const ParsedCommand& cmd);
    void dispatchAttribute(const ParsedCommand& cmd);
    void dispatchMark(const ParsedCommand& cmd);

private:
    ObjectManager& objMgr_;
};
```

### 4.3 LayoutManager

```cpp
class LayoutManager {
public:
    // 布局查询
    InstLayout* getInstLayout(ObjectID instId);
    PinLayout* getPinLayout(ObjectID pinId);
    NetLayout* getNetLayout(ObjectID netId);

    // 布局更新
    void updateInstLayout(ObjectID instId, const QRectF& boundingBox);
    void updatePinLayout(ObjectID pinId, const QPointF& anchorPos, PinSide side, qreal offset);
    void updateNetLayout(ObjectID netId, const std::vector<QPointF>& pathPoints);

    // 自动布局（EDA算法）
    void autoLayout();  // 自动计算布局
    void autoRoute();   // 自动布线

private:
    std::unordered_map<ObjectID, std::unique_ptr<InstLayout>> instLayouts_;
    std::unordered_map<ObjectID, std::unique_ptr<PinLayout>> pinLayouts_;
    std::unordered_map<ObjectID, std::unique_ptr<NetLayout>> netLayouts_;
};
```

---

## 五、ID编码规则

- **ID由外部指定**，协议中传入具体数值
- ID类型通过对象类型区分，非编码在ID中
- 建议ID格式：
  - Inst: `0xxx` (如 `0000001`)
  - Pin: `1xxx` (如 `0000001001`，高1位 + 父InstID低3位)
  - Net: `2xxx` (如 `0000010001`)

---

## 六、EDA布局布线考虑

### 6.1 布局

| 功能 | 方案 |
|------|------|
| Inst重叠检测 | 包围盒碰撞检测 |
| Pin锚点自动计算 | 根据Inst大小和Pin数量平均分配 |
| 网格吸附 | 设定网格大小，坐标取整 |
| 层次边界 | 绘制模块边界框，区分内部/外部视图 |

### 6.2 布线

| 功能 | 方案 |
|------|------|
| 路径计算 | 曼哈顿距离优先 + 避障 |
| 弯折优化 | 减少不必要的折点 |
| 层次Net | 跨层次连线展开处理 |

---

## 七、图形层 (Qt)

### 7.1 显示规格

#### Inst 显示
- **形状**：矩形框
- **name标注**：矩形**上方**，居中
- **module_name标注**：矩形**下方**，居中
- **TOP Inst**：矩形边框为**虚线**（Qt::DashLine）
- **普通Inst**：矩形边框为实线（Qt::SolidLine）
- **默认尺寸**：宽80 x 高60（或根据内容自适应）

#### Pin 显示
- **形状**：
  - Pin本身：短线段（长度约10px）+ 端点标记（小方块或圆点）
  - **位置**：Pin短线段在inst边界外侧
  - **方向**：
    - INPUT → 短线段在inst**左侧**，指向inst
    - OUTPUT → 短线段在inst**右侧**，背离inst

#### 连接状态标记（Pin外侧端点）
- **1个connection**：绿色菱形（8x8px）
- **多个connection**：橙色菱形（8x8px）
- **无connection**：红色圆圈（直径6px）

### 7.2 图形项继承关系

```
QGraphicsItem
├── InstGraphicsItem
│   ├── 矩形框（实线/虚线）
│   ├── name文本（上方）
│   └── module_name文本（下方）
├── PinGraphicsItem
│   ├── 短线段
│   └── 连接状态标记（菱形/圆圈）
└── NetGraphicsItem
    └── 绘制连线（直线/折线）-- 第一阶段暂不做
```

### 7.3 层级管理 (Z-order)

- Net: 0 (底层)
- Pin: 10 (中层)
- Inst: 20 (顶层)
- Selection: 100 (选中高亮)

### 7.4 第一阶段目标

仅实现Inst和Pin的显示：
1. Inst矩形 + name + module_name
2. Pin短线段 + 端点标记
3. 连接状态标记（绿/橙/红）

---

## 八、下一步

1. 创建项目目录结构
2. 实现ObjectManager核心类
3. 实现CommandParser命令解析
4. 集成Qt显示框架
5. 实现最小可运行Demo
