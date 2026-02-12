/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef GRAPHICS_EFFECT_GE_CACHE_PROVIDER_H
#define GRAPHICS_EFFECT_GE_CACHE_PROVIDER_H
#include "ge_downcast.h"

namespace OHOS {
namespace Rosen {

struct IGECache : ExactDowncastUtils::ExactDowncastable {
    virtual ~IGECache() = default;
};

struct IGECacheProvider : ExactDowncastUtils::ExactDowncastable {
    virtual ~IGECacheProvider() = default;
    virtual const IGECache* GetFirst() const = 0;
    virtual bool Store(const IGECache& cache) = 0;
};

template<typename Derived>
struct GECache : IGECache {
    ExactDowncastUtils::TypeID GetExactTypeID() const override
    {
        return ExactDowncastUtils::TypeID::Get<Derived>();
    }
};

template<typename Derived>
struct GECacheProvider : IGECacheProvider {
    ExactDowncastUtils::TypeID GetExactTypeID() const override
    {
        return ExactDowncastUtils::TypeID::Get<Derived>();
    }

    IGECacheProvider* AsBase() const
    {
        return static_cast<IGECacheProvider*>(this);
    }
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_CACHE_PROVIDER_H
