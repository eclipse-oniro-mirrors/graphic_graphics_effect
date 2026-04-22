/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GRAPHICS_EFFECT_GE_EFFECT_FACTORY_H
#define GRAPHICS_EFFECT_GE_EFFECT_FACTORY_H

#include <functional>
#include <array>
#include <memory>
#include <optional>
#include <type_traits>
#include "ge_visual_effect_impl.h"
#include "ge_log.h"

namespace OHOS {
namespace Rosen {
class GEShader;
class GEShaderFilter;
class GEExternalDynamicLoader;
}

namespace GraphicsEffectEngine {

class GEEffectFactory {
public:
    using VisualEffectImplPtr = std::shared_ptr<Rosen::Drawing::GEVisualEffectImpl>;
    using EffectCreator = std::function<std::shared_ptr<Rosen::Drawing::IGEFilterType>(VisualEffectImplPtr)>;

    static GEEffectFactory& GetInstance();

    void Register(Rosen::Drawing::GEFilterType type, EffectCreator&& creator);

    std::shared_ptr<Rosen::Drawing::IGEFilterType> Create(VisualEffectImplPtr impl);

    std::shared_ptr<Rosen::GEShader> CreateShader(VisualEffectImplPtr impl);
    std::shared_ptr<Rosen::GEShaderFilter> CreateFilter(VisualEffectImplPtr impl);
    std::shared_ptr<Rosen::Drawing::GEShaderMask> CreateMask(VisualEffectImplPtr impl);
    std::shared_ptr<Rosen::Drawing::GEShaderShape> CreateShape(VisualEffectImplPtr impl);

    static bool IsFallbackDisabled();

private:
    static constexpr size_t MAX_EFFECTS = static_cast<size_t>(Rosen::Drawing::GEFilterType::MAX);

    GEEffectFactory() = default;
    ~GEEffectFactory() = default;
    GEEffectFactory(const GEEffectFactory&) = delete;
    GEEffectFactory& operator=(const GEEffectFactory&) = delete;

    std::array<std::optional<EffectCreator>, MAX_EFFECTS> creators_;
};

} // namespace GraphicsEffectEngine
} // namespace OHOS

inline bool GE_CheckNullptr(const void* ptr, const char* logTag, int filterType = -1)
{
    if (ptr == nullptr) {
        if (filterType >= 0) {
            GE_LOGE("[EffectFactory] %{public}s: nullptr, type=%{public}d", logTag, filterType);
        } else {
            GE_LOGE("[EffectFactory] %{public}s: nullptr", logTag);
        }
        return true;
    }
    return false;
}

namespace OHOS {
namespace GraphicsEffectEngine {

#define _GE_REGISTER_IMPL(ClassName, FullClassName) \
    namespace { \
        struct GEEffectRegistrar_##ClassName { \
            GEEffectRegistrar_##ClassName() \
            { \
                using Factory = ::OHOS::GraphicsEffectEngine::GEEffectFactory; \
                using IGEFilterPtr = std::shared_ptr<::OHOS::Rosen::Drawing::IGEFilterType>; \
                Factory::GetInstance().Register( \
                    ::OHOS::Rosen::Drawing::GEFilterTypeInfo<FullClassName>::ID, \
                    [](Factory::VisualEffectImplPtr ve) -> IGEFilterPtr { \
                        if (GE_CheckNullptr(ve.get(), #ClassName)) return nullptr; \
                        using ParamType = typename ::OHOS::Rosen::Drawing::GEFilterTypeInfo<FullClassName>::ParamType; \
                        auto params = ve->template GetParams<ParamType>(); \
                        if (GE_CheckNullptr(params.get(), #ClassName ":GetParams", \
                            static_cast<int>(ve->GetFilterType()))) return nullptr; \
                        return std::make_shared<FullClassName>(const_cast<ParamType&>(*params)); \
                    }); \
            } \
        }; \
        static GEEffectRegistrar_##ClassName g_effectRegistrar_##ClassName; \
    }

#define GE_FACTORY_REGISTER(ClassName) _GE_REGISTER_IMPL(ClassName, ::OHOS::Rosen::ClassName)
#define GE_FACTORY_REGISTER_MASK(ClassName) _GE_REGISTER_IMPL(ClassName, ::OHOS::Rosen::Drawing::ClassName)
#define GE_FACTORY_REGISTER_SHAPE(ClassName) _GE_REGISTER_IMPL(ClassName, ::OHOS::Rosen::Drawing::ClassName)

#define GE_FACTORY_REGISTER_EXTERNAL(EffectType, ParamType) \
    namespace { \
        struct GEEffectRegistrar_##EffectType { \
            GEEffectRegistrar_##EffectType() \
            { \
                using Factory = ::OHOS::GraphicsEffectEngine::GEEffectFactory; \
                using IGEFilterPtr = std::shared_ptr<::OHOS::Rosen::Drawing::IGEFilterType>; \
                Factory::GetInstance().Register( \
                    ::OHOS::Rosen::Drawing::GEFilterType::EffectType, \
                    [](Factory::VisualEffectImplPtr ve) -> IGEFilterPtr { \
                        if (GE_CheckNullptr(ve.get(), #EffectType)) return nullptr; \
                        auto params = ve->template GetParams<ParamType>(); \
                        if (GE_CheckNullptr(params.get(), #EffectType ":GetParams", \
                            static_cast<int>(ve->GetFilterType()))) return nullptr; \
                        uint32_t type = static_cast<uint32_t>(::OHOS::Rosen::Drawing::GEFilterType::EffectType); \
                        void* impl = ::OHOS::Rosen::GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType( \
                            type, sizeof(ParamType), const_cast<void*>(static_cast<const void*>(params.get()))); \
                        if (GE_CheckNullptr(impl, #EffectType ":ExternalLoader")) return nullptr; \
                        return std::shared_ptr<::OHOS::Rosen::Drawing::IGEFilterType>( \
                            static_cast<::OHOS::Rosen::Drawing::IGEFilterType*>(impl)); \
                    }); \
            } \
        }; \
        static GEEffectRegistrar_##EffectType g_effectRegistrar_##EffectType; \
    }

#define GE_FACTORY_REGISTER_EXTERNAL_FALLBACK(EffectType, ParamType, FallbackClass) \
    namespace { \
        struct GEEffectRegistrar_##EffectType { \
            GEEffectRegistrar_##EffectType() \
            { \
                using Factory = ::OHOS::GraphicsEffectEngine::GEEffectFactory; \
                using IGEFilterPtr = std::shared_ptr<::OHOS::Rosen::Drawing::IGEFilterType>; \
                Factory::GetInstance().Register( \
                    ::OHOS::Rosen::Drawing::GEFilterType::EffectType, \
                    [](Factory::VisualEffectImplPtr ve) -> IGEFilterPtr { \
                        if (GE_CheckNullptr(ve.get(), #EffectType)) return nullptr; \
                        auto params = ve->template GetParams<ParamType>(); \
                        if (GE_CheckNullptr(params.get(), #EffectType ":GetParams", \
                            static_cast<int>(ve->GetFilterType()))) return nullptr; \
                        static_assert(std::is_base_of_v<::OHOS::Rosen::Drawing::IGEFilterType, FallbackClass>); \
                        uint32_t type = static_cast<uint32_t>(::OHOS::Rosen::Drawing::GEFilterType::EffectType); \
                        void* impl = ::OHOS::Rosen::GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType( \
                            type, sizeof(ParamType), const_cast<void*>(static_cast<const void*>(params.get()))); \
                        if (impl) { \
                            GE_LOGD("[GEEffectFactory] %{public}s: Using external loader", #EffectType); \
                            return std::shared_ptr<::OHOS::Rosen::Drawing::IGEFilterType>( \
                                static_cast<::OHOS::Rosen::Drawing::IGEFilterType*>(impl)); \
                        } \
                        GE_LOGW("[GEEffectFactory] %{public}s: External loader failed, using fallback %{public}s", \
                            #EffectType, #FallbackClass); \
                        return std::make_shared<FallbackClass>(const_cast<ParamType&>(*params)); \
                    }); \
            } \
        }; \
        static GEEffectRegistrar_##EffectType g_effectRegistrar_##EffectType; \
    }

#define GE_FACTORY_REGISTER_CUSTOM(EffectType, Lambda) \
    namespace { \
        struct GEEffectRegistrar_##EffectType { \
            GEEffectRegistrar_##EffectType() \
            { \
                ::OHOS::GraphicsEffectEngine::GEEffectFactory::GetInstance().Register( \
                    ::OHOS::Rosen::Drawing::GEFilterType::EffectType, Lambda); \
            } \
        }; \
        static GEEffectRegistrar_##EffectType g_effectRegistrar_##EffectType; \
    }

} // namespace GraphicsEffectEngine
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_EFFECT_FACTORY_H
