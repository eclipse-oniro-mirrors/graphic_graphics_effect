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
#include <string>
#include <type_traits>
#include "ge_visual_effect_impl.h"
#include "ge_log.h"
#include "ext/ge_external_dynamic_loader.h"

namespace OHOS {
namespace Rosen {
class GEShader;
class GEShaderFilter;
}

namespace GraphicsEffectEngine {

class GEEffectFactory {
public:
    using VisualEffectImplPtr = std::shared_ptr<Rosen::Drawing::GEVisualEffectImpl>;
    using EffectCreator = std::function<std::shared_ptr<Rosen::Drawing::IGEFilterType>(VisualEffectImplPtr)>;

    static void Register(Rosen::Drawing::GEFilterType type, EffectCreator&& creator);

    static std::shared_ptr<Rosen::Drawing::IGEFilterType> Create(VisualEffectImplPtr impl);
    static std::shared_ptr<Rosen::GEShader> CreateShader(VisualEffectImplPtr impl);
    static std::shared_ptr<Rosen::GEShaderFilter> CreateFilter(VisualEffectImplPtr impl);
    static std::shared_ptr<Rosen::Drawing::GEShaderMask> CreateMask(VisualEffectImplPtr impl);
    static std::shared_ptr<Rosen::Drawing::GEShaderShape> CreateShape(VisualEffectImplPtr impl);

private:
    static constexpr size_t MAX_EFFECTS = static_cast<size_t>(Rosen::Drawing::GEFilterType::MAX);
    static std::array<std::optional<EffectCreator>, MAX_EFFECTS> creators_;

    GEEffectFactory() = delete;
    ~GEEffectFactory() = delete;
    GEEffectFactory(const GEEffectFactory&) = delete;
    GEEffectFactory& operator=(const GEEffectFactory&) = delete;
};

} // namespace GraphicsEffectEngine
} // namespace OHOS

inline bool FactoryCheckNullptr(const void* ptr, const char* logTag, int32_t filterType = -1)
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
namespace Internal {  // Internal implementation namespace, not exposed externally

template<typename FullClassName>
void RegisterEffect(const char* logTag)
{
    GEEffectFactory::Register(
        ::OHOS::Rosen::Drawing::GEFilterTypeInfo<FullClassName>::ID,
        [logTag](GEEffectFactory::VisualEffectImplPtr ve)
            -> std::shared_ptr<::OHOS::Rosen::Drawing::IGEFilterType> {
            std::string tag = std::string(logTag) + ":";
            if (FactoryCheckNullptr(ve.get(), logTag)) return nullptr;
            using ParamType = typename ::OHOS::Rosen::Drawing::GEFilterTypeInfo<FullClassName>::ParamType;
            auto params = ve->template GetParams<ParamType>();
            std::string tagGetParams = tag + "GetParams";
            if (FactoryCheckNullptr(params.get(), tagGetParams.c_str(),
                static_cast<int32_t>(ve->GetFilterType()))) return nullptr;
            return std::make_shared<FullClassName>(*params);
        });
}

template<typename ParamType, ::OHOS::Rosen::Drawing::GEFilterType EffectType>
void RegisterExternalEffect(const char* logTag)
{
    GEEffectFactory::Register(EffectType,
        [logTag](GEEffectFactory::VisualEffectImplPtr ve)
            -> std::shared_ptr<::OHOS::Rosen::Drawing::IGEFilterType> {
            std::string tag = std::string(logTag) + ":";
            if (FactoryCheckNullptr(ve.get(), logTag)) return nullptr;
            auto params = ve->template GetParams<ParamType>();
            std::string tagGetParams = tag + "GetParams";
            if (FactoryCheckNullptr(params.get(), tagGetParams.c_str(),
                static_cast<int32_t>(ve->GetFilterType()))) return nullptr;
            uint32_t type = static_cast<uint32_t>(EffectType);
            void* impl = ::OHOS::Rosen::GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
                type, sizeof(ParamType), static_cast<void*>(params.get()));
            std::string tagExternalLoader = tag + "ExternalLoader";
            if (FactoryCheckNullptr(impl, tagExternalLoader.c_str())) return nullptr;
            return std::shared_ptr<::OHOS::Rosen::Drawing::IGEFilterType>(
                static_cast<::OHOS::Rosen::Drawing::IGEFilterType*>(impl));
        });
}

template<typename ParamType, typename FallbackClass, ::OHOS::Rosen::Drawing::GEFilterType EffectType>
void RegisterExternalFallbackEffect(const char* logTag)
{
    static_assert(std::is_base_of_v<::OHOS::Rosen::Drawing::IGEFilterType, FallbackClass>);
    GEEffectFactory::Register(EffectType,
        [logTag](GEEffectFactory::VisualEffectImplPtr ve)
            -> std::shared_ptr<::OHOS::Rosen::Drawing::IGEFilterType> {
            std::string tag = std::string(logTag) + ":";
            if (FactoryCheckNullptr(ve.get(), logTag)) return nullptr;
            auto params = ve->template GetParams<ParamType>();
            std::string tagGetParams = tag + "GetParams";
            if (FactoryCheckNullptr(params.get(), tagGetParams.c_str(),
                static_cast<int32_t>(ve->GetFilterType()))) return nullptr;
            uint32_t type = static_cast<uint32_t>(EffectType);
            void* impl = ::OHOS::Rosen::GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
                type, sizeof(ParamType), static_cast<void*>(params.get()));
            if (impl) {
                GE_LOGD("[GEEffectFactory] %{public}s: Using external loader", logTag);
                return std::shared_ptr<::OHOS::Rosen::Drawing::IGEFilterType>(
                    static_cast<::OHOS::Rosen::Drawing::IGEFilterType*>(impl));
            }
            GE_LOGW("[GEEffectFactory] %{public}s: External failed, using fallback", logTag);
            return std::make_shared<FallbackClass>(*params);
        });
}

} // namespace Internal

#define GE_REGISTER_IMPL(ClassName, FullClassName) \
    namespace { \
        struct GEEffectRegistrar_##ClassName { \
            GEEffectRegistrar_##ClassName() \
            { ::OHOS::GraphicsEffectEngine::Internal::RegisterEffect<FullClassName>(#ClassName); } \
        }; \
        static GEEffectRegistrar_##ClassName g_effectRegistrar_##ClassName; \
    }

#define GE_FACTORY_REGISTER(ClassName) GE_REGISTER_IMPL(ClassName, ::OHOS::Rosen::ClassName)
#define GE_FACTORY_REGISTER_MASK(ClassName) GE_REGISTER_IMPL(ClassName, ::OHOS::Rosen::Drawing::ClassName)
#define GE_FACTORY_REGISTER_SHAPE(ClassName) GE_REGISTER_IMPL(ClassName, ::OHOS::Rosen::Drawing::ClassName)

#define GE_FACTORY_REGISTER_EXTERNAL(EffectType, ParamType) \
    namespace { \
        struct GEEffectRegistrar_##EffectType { \
            GEEffectRegistrar_##EffectType() \
            { ::OHOS::GraphicsEffectEngine::Internal::RegisterExternalEffect<ParamType, \
                ::OHOS::Rosen::Drawing::GEFilterType::EffectType>(#EffectType); } \
        }; \
        static GEEffectRegistrar_##EffectType g_effectRegistrar_##EffectType; \
    }

#define GE_FACTORY_REGISTER_EXTERNAL_FALLBACK(EffectType, ParamType, FallbackClass) \
    namespace { \
        struct GEEffectRegistrar_##EffectType { \
            GEEffectRegistrar_##EffectType() \
            { ::OHOS::GraphicsEffectEngine::Internal::RegisterExternalFallbackEffect<ParamType, FallbackClass, \
                ::OHOS::Rosen::Drawing::GEFilterType::EffectType>(#EffectType); } \
        }; \
        static GEEffectRegistrar_##EffectType g_effectRegistrar_##EffectType; \
    }

#define GE_FACTORY_REGISTER_CUSTOM(EffectType, Lambda) \
    namespace { \
        struct GEEffectRegistrar_##EffectType { \
            GEEffectRegistrar_##EffectType() \
            { ::OHOS::GraphicsEffectEngine::GEEffectFactory::Register( \
                ::OHOS::Rosen::Drawing::GEFilterType::EffectType, (Lambda)); } \
        }; \
        static GEEffectRegistrar_##EffectType g_effectRegistrar_##EffectType; \
    }

} // namespace GraphicsEffectEngine
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_EFFECT_FACTORY_H