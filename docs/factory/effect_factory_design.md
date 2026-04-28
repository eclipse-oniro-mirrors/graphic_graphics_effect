# 效果工厂设计文档

## 一、概述

### 1.1 什么是效果工厂？

效果工厂（`GEEffectFactory`）是 Graphics Effect（GE）组件的核心基础设施，提供统一的效果创建和管理机制：

- **集中注册**：所有效果类型在一个文件中集中注册
- **O(1)查找**：通过数组索引快速查找和创建效果
- **统一接口**：提供统一的效果创建接口
- **无回退**：工厂完全接管效果创建，无 switch-case 回退

### 1.2 设计目标

1. **代码维护**：新增效果只需添加一行注册代码
2. **扩展性**：支持闭源动态加载效果
3. **兼容性**：完全兼容现有的参数系统
4. **性能优化**：消除哈希表开销（shaderCreatorLUT）

### 1.3 适用场景

- 需要频繁创建效果的渲染管线
- 支持动态加载闭源效果库
- 需要统一管理大量效果类型
- 需要快速查找和创建特定效果

---

## 二、架构设计

### 2.1 纯静态类架构

```
┌─────────────────────────────────────────────────────────────┐
│                    GEEffectFactory (Static Class)            │
│  ┌─────────────────────────────────────────────────────┐   │
│  │              Static Methods                          │   │
│  │  - Register(type, creator)                          │   │
│  │  - Create/CreateFilter/CreateMask/CreateShape       │   │
│  └─────────────────────────────────────────────────────┘   │
│  ┌─────────────────────────────────────────────────────┐   │
│  │              Static Registration Table               │   │
│  │  - std::array<std::optional<Creator>, MAX>          │   │
│  └─────────────────────────────────────────────────────┘   │
│  ┌─────────────────────────────────────────────────────┐   │
│  │              Creator Functions                       │   │
│  │  - Lambda functions for each effect type            │   │
│  │  - Call GetParams<T>() and make_shared              │   │
│  └─────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 文件组织

```
graphics_effect/
├── include/core/
│   └── ge_effect_factory.h          # 工厂类定义和注册宏
├── src/core/
│   ├── ge_effect_factory.cpp        # 工厂实现
│   ├── ge_factory_register.cpp      # 集中注册（字母序）
│   └── ge_visual_effect.cpp         # 工厂调用集成
└── test/unittest/
    └── ge_effect_factory_test.cpp   # TDD测试
```

**设计原则：**
- 工厂类作为核心基础设施，放在 `core` 目录
- 注册代码分离到独立文件，便于维护和减少编辑冲突
- **支持分散注册**：每个效果可在自己的 cpp 文件中注册（静态对象自动注册）
- **推荐集中注册**：当前采用集中方式，便于统一管理和查找
- 头文件引用按字母序排列，符合代码规范
- 注册按功能分组：Filter → Shader → Mask → Shape
- 子分类内按字母序排序，便于查找和维护

### 2.3 渲染流程

```
┌──────────────┐
│ 用户请求效果  │
└───────┬──────┘
        │
        ├─────────────────────────────────┐
        │                                 │
        ▼                                 ▼
┌──────────────────┐              ┌──────────────────┐
│    GERender      │              │  GEVisualEffect  │
│ GenerateFilter   │              │ GenerateMask     │
│ GenerateShader   │              │ GenerateShape    │
└──────┬───────────┘              └───────┬──────────┘
        │                                 │
        └────────────────┬────────────────┘
                         │
                         ▼
               ┌──────────────────┐
               │  GEEffectFactory │
               │  CreateFilter    │
               │  CreateShader    │
               │  CreateMask      │
               │  CreateShape     │
               └──────┬───────────┘
                      │
                      ├─────────────┬─────────────┐
                      │             │             │
                      ▼             ▼             ▼
                ┌─────────┐  ┌──────────┐  ┌────────────┐
                │数组索引  │  │GetParams │  │make_shared │
                │O(1)     │  │<ParamType>│  │<ClassName> │
                └─────────┘  └──────────┘  └────────────┘
                      │             │             |
                      └─────────────┴─────────────┘
                                    │
                                    ▼
                          ┌──────────────────┐
                          │ 效果对象返回      │
                          └──────────────────┘
```

**调用路径：**
- **Filter/Shader效果**：通过 `GERender::GenerateShaderFilter()` / `GenerateShaderEffect()` 调用工厂
- **Mask/Shape效果**：通过 `GEVisualEffect::GenerateShaderMask()` / `GenerateShaderShape()` 调用工厂

---

## 三、核心接口

### 3.1 工厂类接口（纯静态）

```cpp
namespace OHOS {
namespace GraphicsEffectEngine {

class GEEffectFactory {
public:
    using VisualEffectImplPtr = std::shared_ptr<Rosen::Drawing::GEVisualEffectImpl>;
    using EffectCreator = std::function<std::shared_ptr<Rosen::Drawing::IGEFilterType>(VisualEffectImplPtr)>;

    // 注册接口（静态）
    static void Register(Rosen::Drawing::GEFilterType type, EffectCreator&& creator);

    // 创建效果接口（静态）
    static std::shared_ptr<Rosen::Drawing::IGEFilterType> Create(VisualEffectImplPtr impl);
    static std::shared_ptr<Rosen::GEShader> CreateShader(VisualEffectImplPtr impl);
    static std::shared_ptr<Rosen::GEShaderFilter> CreateFilter(VisualEffectImplPtr impl);
    static std::shared_ptr<Rosen::Drawing::GEShaderMask> CreateMask(VisualEffectImplPtr impl);
    static std::shared_ptr<Rosen::Drawing::GEShaderShape> CreateShape(VisualEffectImplPtr impl);

private:
    static constexpr size_t MAX_EFFECTS = static_cast<size_t>(Rosen::Drawing::GEFilterType::MAX);
    GEEffectFactory() = delete;  // 禁止实例化

    // 注册表（静态数组，O(1) 查找）
    static std::array<std::optional<EffectCreator>, MAX_EFFECTS> creators_;
};

} // namespace GraphicsEffectEngine
} // namespace OHOS
```

### 3.2 注册宏接口

**核心逻辑提取到 `Internal` 命名空间的模板函数，宏定义精简到≤10行：**

```cpp
namespace OHOS {
namespace GraphicsEffectEngine {
namespace Internal {

template<typename FullClassName>
void RegisterEffect(const char* logTag);

template<typename ParamType, GEFilterType EffectType>
void RegisterClosedSourceEffect(const char* logTag);

template<typename ParamType, typename FallbackClass, GEFilterType EffectType>
void RegisterClosedSourceFallbackEffect(const char* logTag);

} // namespace Internal
} // GraphicsEffectEngine
}

// 简化的宏定义（≤10行）
#define GE_REGISTER_IMPL(ClassName, FullClassName) \
    namespace { \
        struct GEEffectRegistrar_##ClassName { \
            GEEffectRegistrar_##ClassName() { Internal::RegisterEffect<FullClassName>(#ClassName); } \
        }; \
        static GEEffectRegistrar_##ClassName g_effectRegistrar_##ClassName; \
    }

#define GE_FACTORY_REGISTER(ClassName) GE_REGISTER_IMPL(ClassName, ::OHOS::Rosen::ClassName)
#define GE_FACTORY_REGISTER_MASK(ClassName) GE_REGISTER_IMPL(ClassName, ::OHOS::Rosen::Drawing::ClassName)
#define GE_FACTORY_REGISTER_SHAPE(ClassName) GE_REGISTER_IMPL(ClassName, ::OHOS::Rosen::Drawing::ClassName)
```

**设计要点：**
- 模板函数放在 `Internal` 命名空间，明确标识为内部实现
- 核心逻辑提取到模板函数，宏≤10行符合规范
- `##ClassName`/`#ClassName` 无法加括号（C++语法限制）

---

## 四、参数系统集成

工厂宏依赖的接口（全部已存在）：

1. **DECLARE_GEFILTER_TYPEFUNC** - 效果类中定义
2. **GEFilterTypeInfo<T>::ParamType** - 自动提取参数类型
3. **GEFilterParamsTypeInfo<ParamType>::ID** - 自动获取枚举值
4. **GetParams<T>()** - 模板方法获取参数

**无需额外适配**，工厂宏自动完成所有工作。

---

## 五、性能分析

### 5.1 性能对比

| 方式 | 查找复杂度 | 添加新效果 | 维护成本 |
|------|-----------|----------|----------|
| **Switch-case（跳转表）** | O(1) | 修改多处代码 | 高 - 分散维护 |
| **shaderCreatorLUT（哈希表）** | 平均O(1)，最坏O(n) | 修改多处代码 | 高 |
| **工厂模式（纯静态）** | O(1) - 数组索引 | 添加一行注册 | 低 - 集中维护 |

### 5.2 性能优势

1. **消除哈希开销**：无哈希计算、无初始化构建
2. **稳定查找**：数组索引 O(1)，无最坏 O(n) 情况
3. **内存占用**：注册表大小固定，Lambda 函数编译期优化
4. **直接调用**：纯静态方法，无需单例间接调用

---

## 六、线程安全

### 6.1 静态初始化线程安全

工厂使用静态成员变量，C++11 标准保证线程安全初始化：

```cpp
// 静态成员定义
std::array<std::optional<EffectCreator>, MAX_EFFECTS> GEEffectFactory::creators_;
```

### 6.2 操作线程安全分析

| 操作 | 线程安全性 | 说明 |
|------|----------|------|
| `Register()` | ✅ 安全 | 只在静态初始化阶段调用（程序启动前） |
| `Create()` | ✅ 安全 | 只读操作，访问静态 `creators_[]` 数组 |

---

## 七、扩展机制

### 7.1 闭源动态加载

支持动态加载闭源效果库：

```cpp
void* impl = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
    static_cast<uint32_t>(type), sizeof(ParamType), params);
```

### 7.2 闭源效果库要求

- 导出 `CreateGEXObjectByType` 函数
- 支持参数序列化/反序列化
- 符合效果接口规范

---

## 附录A：文件清单

| 文件 | 路径 | 说明 |
|------|------|------|
| 工厂头文件 | include/core/ge_effect_factory.h | 工厂类定义和注册宏 |
| 工厂实现 | src/core/ge_effect_factory.cpp | 工厂实现 |
| 集中注册 | src/core/ge_factory_register.cpp | 效果集中注册（字母序） |
| 工厂测试 | test/unittest/ge_effect_factory_test.cpp | TDD测试 |


---

**文档版本：** v1.1
**最后更新：** 2026-04-27
**维护者：** Graphics Effect Team
