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

#include "core/ge_effect_factory.h"
#include "ge_system_properties.h"
#include <memory>

namespace OHOS {
namespace GraphicsEffectEngine {

std::array<std::optional<GEEffectFactory::EffectCreator>, GEEffectFactory::MAX_EFFECTS> GEEffectFactory::creators_;

void GEEffectFactory::Register(Rosen::Drawing::GEFilterType type, EffectCreator&& creator)
{
    if (type == Rosen::Drawing::GEFilterType::NONE) {
        return;  // NONE is invalid type
    }
    const size_t index = static_cast<size_t>(type);
    if (index >= MAX_EFFECTS) {
        return;  // out of range
    }
    if (creators_[index]) {
        GE_LOGE("[GEEffectFactory] Duplicate registration for type=%{public}d, "
            "previous registration is kept. Please check if multiple files register the same effect.",
            static_cast<int>(type));
        return;
    }
    creators_[index] = std::forward<EffectCreator>(creator);
}

std::shared_ptr<Rosen::Drawing::IGEFilterType> GEEffectFactory::Create(
    std::shared_ptr<Rosen::Drawing::GEVisualEffectImpl> impl)
{
    if (FactoryCheckNullptr(impl.get(), "Create")) {
        return nullptr;
    }

    const auto filterType = impl->GetFilterType();
    const size_t index = static_cast<size_t>(filterType);

    if (index >= MAX_EFFECTS) {
        GE_LOGW("[GEEffectFactory] Create: type=%{public}d out of range", static_cast<int>(filterType));
        return nullptr;
    }

    if (!creators_[index]) {
        GE_LOGE("[GEEffectFactory] Create: no creator for type=%{public}d, please register with GE_FACTORY_REGISTER",
            static_cast<int>(filterType));
        return nullptr;
    }

    auto result = (*creators_[index])(impl);
    GE_LOGD("[GEEffectFactory] Create: type=%{public}d result=%{public}s",
        static_cast<int>(filterType), result ? "valid" : "nullptr");
    return result;
}

std::shared_ptr<Rosen::GEShader> GEEffectFactory::CreateShader(
    std::shared_ptr<Rosen::Drawing::GEVisualEffectImpl> impl)
{
    auto result = Create(impl);
    if (!result) {
        return nullptr;
    }
    return std::static_pointer_cast<Rosen::GEShader>(result);
}

std::shared_ptr<Rosen::GEShaderFilter> GEEffectFactory::CreateFilter(
    std::shared_ptr<Rosen::Drawing::GEVisualEffectImpl> impl)
{
    auto result = Create(impl);
    if (!result) {
        return nullptr;
    }
    return std::static_pointer_cast<Rosen::GEShaderFilter>(result);
}

std::shared_ptr<Rosen::Drawing::GEShaderMask> GEEffectFactory::CreateMask(
    std::shared_ptr<Rosen::Drawing::GEVisualEffectImpl> impl)
{
    auto result = Create(impl);
    if (!result) {
        return nullptr;
    }
    return std::static_pointer_cast<Rosen::Drawing::GEShaderMask>(result);
}

std::shared_ptr<Rosen::Drawing::GEShaderShape> GEEffectFactory::CreateShape(
    std::shared_ptr<Rosen::Drawing::GEVisualEffectImpl> impl)
{
    auto result = Create(impl);
    if (!result) {
        return nullptr;
    }
    return std::static_pointer_cast<Rosen::Drawing::GEShaderShape>(result);
}

} // namespace GraphicsEffectEngine
} // namespace OHOS