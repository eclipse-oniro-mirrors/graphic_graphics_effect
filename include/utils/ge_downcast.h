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
#ifndef GRAPHICS_EFFECT_GE_DOWNCAST_H
#define GRAPHICS_EFFECT_GE_DOWNCAST_H
#include <cstdint>
namespace OHOS {
namespace Rosen {

// Helper classes & functions for *exact* type check of downcasting
// Safety:
// - Exception-free
// - RTTI-free
//
// Limitation:
// Unlike full `dynamic_cast`, this performs an exact type check, which means casting to types
// not matched with the *exact* type id will fail, including casting from derived type to parent type.
// In other words, `is-a` subclass traversal is NOT supported.
struct ExactDowncastUtils {
    // We use a 64-bit FNV-1a hash function to generate a unique id of each class in compile time.
    // 64-bit hashing is statistically safe to avoid collisions in our compile-time typeid use case.
    template<size_t LEN>
    static constexpr uint64_t CompileTimeHash(const char (&str)[LEN])
    {
        // FNV-1a 64-bit constants
        constexpr uint64_t prime = 0x100000001b3ULL;
        constexpr uint64_t basis = 0xcbf29ce484222325ULL;

        uint64_t hash = basis;
        for (std::size_t i = 0; i < LEN; ++i) {
            hash ^= str[i];
            hash *= prime;
        }
        return hash;
    }

    // A type-safe wrapper to avoid accidentially cast from raw int
    struct TypeID {
        using RawTypeID = uint64_t;
        explicit TypeID(RawTypeID impl) : id(impl) {}
        bool operator==(const TypeID& other) const
        {
            return id == other.id;
        }

        template<typename T>
        static TypeID Get()
        {
            constexpr RawTypeID typeHash = CompileTimeHash(__PRETTY_FUNCTION__);
            return TypeID(typeHash);
        };

        RawTypeID id;
    };

    // The interface of exact downcast
    class ExactDowncastable {
    public:
        virtual ~ExactDowncastable() = default;
        virtual TypeID GetExactTypeID() const = 0;

        // Check if this object is EXACTLY type T
        template<typename T>
        bool Is() const
        {
            return this->GetExactTypeID() == TypeID::Get<T>();
        }

        // Safe downcast (returns nullptr on failure)
        template<typename T>
        T* As()
        {
            if (this->Is<T>()) {
                return static_cast<T*>(this);
            }
            return nullptr;
        }

        // Const version of safe downcast (returns nullptr on failure)
        template<typename T>
        const T* As() const
        {
            if (this->Is<T>()) {
                return static_cast<const T*>(this);
            }
            return nullptr;
        }
    };
};

} // namespace Rosen
} // namespace OHOS
#endif // GRAPHICS_EFFECT_GE_DOWNCAST_H