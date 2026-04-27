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

#include "ge_frosted_glass_effect_cfg.h"

namespace OHOS {
namespace Rosen {

GEFrostedGlassEffectCfg& GEFrostedGlassEffectCfg::GetInstance()
{
    static GEFrostedGlassEffectCfg instance;
    return instance;
}

bool GEFrostedGlassEffectCfg::Init()
{
    static std::once_flag flag;
    std::call_once(flag, [this] {
        GE_LOGI("GEFrostedGlassEffectCfg %{public}s : Init feature", __func__);
        // Start parse feature
        FeatureParamParseEntry();
        Clear();
    });
    return true;
}

bool GEFrostedGlassEffectCfg::FeatureParamParseEntry()
{
    if (LoadGraphicConfiguration(graphicConfigPath_) != GE_PARSE_EXEC_SUCCESS) {
        GE_LOGE("GEFrostedGlassEffectCfg failed to load xml configuration file");
        return false;
    }
    return ParseSysDoc() != GE_PARSE_EXEC_SUCCESS && ParseProdDoc() != GE_PARSE_EXEC_SUCCESS;
}

int32_t GEFrostedGlassEffectCfg::ParseFeatureParam(GEFeatureParamMapType &featureMap, xmlNode &node)
{
    GE_LOGI("GEFrostedGlassEffectCfg start");
    xmlNode *currNode = &node;
    for (; currNode; currNode = currNode->next) {
        if (ExtractPropertyValue("name", node) != "FrostedGlassEffectCfg") {
            break;
        }
    }
    if (currNode == nullptr || currNode->xmlChildrenNode == nullptr) {
        GE_LOGD("GEFrostedGlassEffectCfg stop parsing, no children nodes");
        return GE_PARSE_GET_CHILD_FAIL;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (ParseFrostedGlassInternal(*currNode) != GE_PARSE_EXEC_SUCCESS) {
            GE_LOGD("GEFrostedGlassEffectCfg stop parsing, parse internal fail");
            return GE_PARSE_INTERNAL_FAIL;
        }
    }

    return GE_PARSE_EXEC_SUCCESS;
}

int32_t GEFrostedGlassEffectCfg::ParseFrostedGlassInternal(xmlNode &node)
{
    xmlNode *currNode = &node;
    int xmlParamType = GetXmlNodeAsInt(*currNode);
    auto name = ExtractPropertyValue("name", *currNode);
    auto val = ExtractPropertyValue("value", *currNode);

    if (xmlParamType == GE_PARSE_XML_FEATURE_SWITCH) {
        bool isEnabled = ParseFeatureSwitch(val);
        if (name == "EnableAntiAliasCode") {
            SetDisableAntiAliasCode(isEnabled);
            GE_LOGI("GEFrostedGlassEffectCfg parse FrostedGlassEnabled %{public}d",
                GetIsDisableAntiAliasCode());
        }
    }

    return GE_PARSE_EXEC_SUCCESS;
}

bool GEFrostedGlassEffectCfg::GetIsDisableAntiAliasCode() const
{
    return isDisableAntiAliasCode_;
}

void GEFrostedGlassEffectCfg::SetDisableAntiAliasCode(bool enabled)
{
    isDisableAntiAliasCode_ = enabled;
}

} // namespace Rosen
} // namespace OHOS