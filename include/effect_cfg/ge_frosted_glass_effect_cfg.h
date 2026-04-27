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

#ifndef GRAPHICS_EFFECT_GE_FROSTED_GLASS_EFFECT_CFG_H
#define GRAPHICS_EFFECT_GE_FROSTED_GLASS_EFFECT_CFG_H

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "ge_common.h"
#include "ge_log.h"
#include "ge_xml_parser_base.h"

namespace OHOS {
namespace Rosen {

class GE_EXPORT GEFrostedGlassEffectCfg : public GEXmlParserBase {
public:
    static GEFrostedGlassEffectCfg& GetInstance();

    int32_t ParseFeatureParam(GEFeatureParamMapType &featureMap, xmlNode &node) override;

    bool GetIsDisableAntiAliasCode() const;
    void SetDisableAntiAliasCode(bool enabled);
    bool Init();
private:
    GEFrostedGlassEffectCfg() = default;
    ~GEFrostedGlassEffectCfg() = default;
    GEFrostedGlassEffectCfg(const GEFrostedGlassEffectCfg&) = delete;
    GEFrostedGlassEffectCfg& operator=(const GEFrostedGlassEffectCfg&) = delete;

    int32_t ParseFrostedGlassInternal(xmlNode &node);
    bool FeatureParamParseEntry();
    inline static bool isDisableAntiAliasCode_ = false;
    std::string graphicConfigPath_ = "etc/graphic/graphic_config.xml";
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_FROSTED_GLASS_EFFECT_CFG_H