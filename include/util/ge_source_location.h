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

#ifndef GRAPHICS_EFFECT_SOURCE_LOCATION_H
#define GRAPHICS_EFFECT_SOURCE_LOCATION_H

#include <cstdint>

#if defined(__has_builtin)
#if __has_builtin(__builtin_COLUMN)
#define GE_BUILTIN_COLUMN() __builtin_COLUMN()
#endif
#endif
#ifndef GE_BUILTIN_COLUMN
#define GE_BUILTIN_COLUMN() 0
#endif

namespace OHOS {
namespace Rosen {

/**
 * @brief GESourceLocation provides source code location information similar to C++20's std::source_location.
 *
 * This class captures the file name, function name, line number, and column number
 * at the point where it is constructed. It uses compiler builtins (__builtin_FILE,
 * __builtin_FUNCTION, __builtin_LINE, __builtin_COLUMN) to achieve this without
 * requiring C++20.
 *
 * Usage example:
 * @code
 * void LogWithLocation(const std::string& message, const GESourceLocation& loc = GESourceLocation::Current()) {
 *     std::cout << "[" << loc.FileName() << ":" << loc.Line() << "] " << message << std::endl;
 * }
 *
 * LogWithLocation("Error occurred"); // Automatically captures caller's location
 * @endcode
 */
class GESourceLocation {
public:
    /**
     * @brief Creates a GESourceLocation object capturing the current source location.
     *
     * This static method uses default parameters with compiler builtins to capture
     * the location at the call site, not at the definition site.
     *
     * @param file The source file name (default: __builtin_FILE())
     * @param function The function name (default: __builtin_FUNCTION())
     * @param line The line number (default: __builtin_LINE())
     * @param column The column number (default: __builtin_COLUMN())
     * @return GESourceLocation object with captured location information
     */
    static constexpr GESourceLocation Current(const char* file = __builtin_FILE(),
        const char* function = __builtin_FUNCTION(), uint32_t line = __builtin_LINE(),
        uint32_t column = GE_BUILTIN_COLUMN()) noexcept
    {
        return GESourceLocation(file, function, line, column);
    }

    /**
     * @brief Default constructor creates an empty/invalid source location.
     */
    constexpr GESourceLocation() noexcept = default;

    /**
     * @brief Returns the source file name.
     * @return Pointer to null-terminated string containing the file name, or "" if not set.
     */
    constexpr const char* FileName() const noexcept
    {
        return fileName_;
    }

    /**
     * @brief Returns the function name.
     * @return Pointer to null-terminated string containing the function name, or "" if not set.
     */
    constexpr const char* FunctionName() const noexcept
    {
        return functionName_;
    }

    /**
     * @brief Returns the line number.
     * @return Line number in the source file, or 0 if not set.
     */
    constexpr uint32_t Line() const noexcept
    {
        return line_;
    }

    /**
     * @brief Returns the column number.
     * @return Column number in the source file, or 0 if not set.
     */
    constexpr uint32_t Column() const noexcept
    {
        return column_;
    }

    /**
     * @brief Checks if this source location is valid (has meaningful data).
     * @return true if line number is non-zero, false otherwise.
     */
    constexpr bool IsValid() const noexcept
    {
        return line_ != 0;
    }

private:
    /**
     * @brief Private constructor for creating a source location with specific values.
     * @param file The source file name
     * @param function The function name
     * @param line The line number
     * @param column The column number
     */
    constexpr GESourceLocation(const char* file, const char* function, uint32_t line, uint32_t column) noexcept
        : fileName_(file), functionName_(function), line_(line), column_(column)
    {}

    const char* fileName_ = "";
    const char* functionName_ = "";
    uint32_t line_ = 0;
    uint32_t column_ = 0;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_SOURCE_LOCATION_H