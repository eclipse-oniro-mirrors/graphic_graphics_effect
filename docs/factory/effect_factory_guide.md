# 效果工厂使用指南

## 一、快速开始

最简单的注册方式 - 一行代码：

```cpp
// src/core/ge_factory_register.cpp

GE_FACTORY_REGISTER(GEGreyShaderFilter)
```

---

## 二、基础注册

### 2.1 Filter效果

```cpp
GE_FACTORY_REGISTER(GEGreyShaderFilter)
GE_FACTORY_REGISTER(GEMESABlurShaderFilter)
GE_FACTORY_REGISTER(GEColorGradientShaderFilter)
```

### 2.2 Mask效果

```cpp
GE_FACTORY_REGISTER_MASK(GERippleShaderMask)
GE_FACTORY_REGISTER_MASK(GEDoubleRippleShaderMask)
```

### 2.3 Shape效果

```cpp
GE_FACTORY_REGISTER_SHAPE(GESDFRRectShaderShape)
GE_FACTORY_REGISTER_SHAPE(GESDFUnionOpShaderShape)
```

---

## 三、外部效果注册

### 3.1 纯外部加载

效果完全由外部库提供，无内置实现：

```cpp
GE_FACTORY_REGISTER_EXTERNAL(DOT_MATRIX, Drawing::GEDotMatrixShaderParams)
GE_FACTORY_REGISTER_EXTERNAL(HARMONIUM_EFFECT, Drawing::GEHarmoniumEffectShaderParams)
```

### 3.2 外部加载 + 内置回退

优先尝试外部加载，失败时使用内置实现：

```cpp
GE_FACTORY_REGISTER_EXTERNAL_FALLBACK(MESA_BLUR,
    Drawing::GEMESABlurShaderFilterParams,
    GEMESABlurShaderFilter)
```

---

## 四、自定义注册

复杂效果需要自定义创建逻辑时使用。

### 示例：KAWASE_BLUR

**源码位置：** `src/core/ge_factory_register.cpp:145-170`

```cpp
GE_FACTORY_REGISTER_CUSTOM(KAWASE_BLUR, [](auto ve) {
    auto params = ve->GetParams<Drawing::GEKawaseBlurShaderFilterParams>();
    if (!GERender::IsMesablurAllEnabled()) {
        return std::make_shared<GEKawaseBlurShaderFilter>(*params);
    }
    // 尝试外部加载或使用内置 MESA_BLUR
    ...
})
```

---

## 五、添加新效果

### 步骤1：确认 ParamType 定义

效果类头文件中：

```cpp
DECLARE_GEFILTER_TYPEFUNC(GENewEffect, Drawing::GENewEffectParams);
```

### 步骤2：确认参数注册

`ge_shader_filter_params.h` 中：

```cpp
GE_PARAMS_TYPE_INFO(GENewEffectParams, NEW_EFFECT, NewEffect);
```

### 步骤3：添加工厂注册

`src/core/ge_factory_register.cpp` 中，在对应分类内按字母序添加：

```cpp
// Filter效果（字母序）
GE_FACTORY_REGISTER(GENewEffect)
```

### 步骤4：编译验证

```bash
hb build graphics_effect -i
```

### 步骤5：测试验证

```bash
hb build graphics_effect -t
```

---

## 六、常见问题

### Q: "ParamType not found"

效果类缺少 ParamType 定义。

**解决：** 在效果类头文件中添加：
```cpp
DECLARE_GEFILTER_TYPEFUNC(GEYourEffect, Drawing::GEYourEffectParams);
```

### Q: "GetParams not found"

参数类型未注册或类型错误。

**解决：** 检查参数类型，确保 `GE_PARAMS_TYPE_INFO` 已注册。

### Q: "No creator for type"

效果未在工厂注册。

**解决：** 在 `ge_factory_register.cpp` 中添加对应注册。

---

## 七、关键文件

| 文件 | 路径 | 用途 |
|------|------|------|
| 工厂头文件 | `include/core/ge_effect_factory.h` | 类定义和注册宏 |
| 工厂实现 | `src/core/ge_effect_factory.cpp` | 工厂逻辑 |
| 效果注册 | `src/core/ge_factory_register.cpp` | 所有效果注册 |
| 工厂测试 | `test/unittest/ge_effect_factory_test.cpp` | TDD测试 |

---

**文档版本：** v1.0
**最后更新：** 2026-04-25
**维护者：** Graphics Effect Team