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

GEEffectFactory& GEEffectFactory::GetInstance()
{
    static GEEffectFactory instance;
    return instance;
}

void GEEffectFactory::Register(Rosen::Drawing::GEFilterType type, EffectCreator&& creator)
{
    const size_t index = static_cast<size_t>(type);
    if (index >= MAX_EFFECTS) {
        return;
    }
    creators_[index] = std::forward<EffectCreator>(creator);
}

std::shared_ptr<Rosen::Drawing::IGEFilterType> GEEffectFactory::Create(
    std::shared_ptr<Rosen::Drawing::GEVisualEffectImpl> impl)
{
    if (GE_CheckNullptr(impl.get(), "Create")) {
        return nullptr;
    }

    const auto filterType = impl->GetFilterType();
    const size_t index = static_cast<size_t>(filterType);

    if (index >= MAX_EFFECTS) {
        GE_LOGW("[GEEffectFactory] Create: type=%{public}d out of range", static_cast<int>(filterType));
        return nullptr;
    }

    if (!creators_[index]) {
        GE_LOGD("[GEEffectFactory] Create: no creator for type=%{public}d", static_cast<int>(filterType));
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