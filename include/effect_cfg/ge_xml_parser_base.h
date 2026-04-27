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

#ifndef GRAPHICS_EFFECT_GE_XML_PARSER_BASE_H
#define GRAPHICS_EFFECT_GE_XML_PARSER_BASE_H
#include <memory>
#include <string>
#include <unordered_map>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "ge_log.h"

namespace OHOS {
namespace Rosen {

using GEFeatureParamMapType = std::unordered_map<std::string, std::shared_ptr<void>>;

enum GEParseXmlNode {
    GE_PARSE_XML_UNDEFINED = 0,
    GE_PARSE_XML_FEATURE_SWITCH,
    GE_PARSE_XML_FEATURE_SINGLEPARAM,
    GE_PARSE_XML_FEATURE_MULTIPARAM,
};

enum GEParseErrCode {
    GE_PARSE_ERROR = -1,
    GE_PARSE_EXEC_SUCCESS = 0,
    GE_PARSE_NO_PARAM = 100,
    GE_PARSE_SYS_FILE_LOAD_FAIL = 200,
    GE_PARSE_PROD_FILE_LOAD_FAIL,
    GE_PARSE_GET_ROOT_FAIL,
    GE_PARSE_GET_CHILD_FAIL,
    GE_PARSE_INTERNAL_FAIL,
};

class GEXmlParserBase {
public:
    void Destroy();

    virtual ~GEXmlParserBase()
    {
        Destroy();
    }

    virtual int32_t ParseFeatureParam(GEFeatureParamMapType &featureMap, xmlNode &node)
    {
        return GE_PARSE_EXEC_SUCCESS;
    }

    int32_t LoadGraphicConfiguration(const std::string& fileDir);
    int32_t ParseSysDoc();
    int32_t ParseProdDoc();
    bool ParseFeatureSwitch(std::string val);
    std::string ExtractPropertyValue(const std::string &propName, xmlNode &node);
    int32_t GetXmlNodeAsInt(xmlNode &node);
    static bool IsNumber(const std::string& str);
    void Clear();
private:
    int32_t LoadSysConfiguration(const std::string& fileDir);
    void LoadProdConfiguration(const std::string& fileDir);
    bool ParseInternal(xmlNode &node);
    xmlDoc *xmlSysDocument_ = nullptr;
    xmlDoc *xmlProdDocument_ = nullptr;
    std::vector<std::string> sysPaths_ = {"/system/variant/phone/base/", "/system/variant/tablet/base/",
        "/system/variant/pc/base/", "/system/variant/watch/base/", "/system/variant/tv/base/",
        "/system/variant/car/base/", "/system/variant/smarthomehost/base/"};
    std::string prodPath_ = "/sys_prod/";
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_XML_PARSER_BASE_H