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

#include "ge_xml_parser_base.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t XML_STRING_MAX_LENGTH = 20;
}

void GEXmlParserBase::Destroy()
{
    Clear();
}

void GEXmlParserBase::Clear()
{
    if (xmlSysDocument_ != nullptr) {
        xmlFreeDoc(xmlSysDocument_);
        xmlSysDocument_ = nullptr;
    }
    if (xmlProdDocument_ != nullptr) {
        xmlFreeDoc(xmlProdDocument_);
        xmlProdDocument_ = nullptr;
    }
}

int32_t GEXmlParserBase::LoadGraphicConfiguration(const std::string& fileDir)
{
    GE_LOGI("GEXmlParserBase opening xml file");
    // System base config file read
    if (LoadSysConfiguration(fileDir) != GE_PARSE_EXEC_SUCCESS) {
        return GE_PARSE_SYS_FILE_LOAD_FAIL;
    }
    // For different feature settings in variant products
    LoadProdConfiguration(fileDir);
    return GE_PARSE_EXEC_SUCCESS;
}

int32_t GEXmlParserBase::LoadSysConfiguration(const std::string& fileDir)
{
    for (const auto& sysPath : sysPaths_) {
        std::string fullPath = sysPath + fileDir;
        xmlSysDocument_ = xmlParseFile(fullPath.c_str());
        if (xmlSysDocument_ != nullptr) {
            return GE_PARSE_EXEC_SUCCESS;
        }
    }
    GE_LOGE("GEXmlParserBase load sys config fail");
    return GE_PARSE_SYS_FILE_LOAD_FAIL;
}

void GEXmlParserBase::LoadProdConfiguration(const std::string& fileDir)
{
    std::string fullPath = prodPath_ + fileDir;
    xmlProdDocument_ = xmlParseFile(fullPath.c_str());
    if (xmlProdDocument_ != nullptr) {
    }
}

int32_t GEXmlParserBase::ParseSysDoc()
{
    if (xmlSysDocument_ == nullptr) {
        GE_LOGE("GEXmlParserBase parse sys doc fail, document is null");
        return GE_PARSE_SYS_FILE_LOAD_FAIL;
    }

    xmlNode *root = xmlDocGetRootElement(xmlSysDocument_);
    if (root == nullptr) {
        GE_LOGE("GEXmlParserBase parse sys doc fail, root is null");
        return GE_PARSE_GET_ROOT_FAIL;
    }

    return ParseInternal(*root) ? GE_PARSE_EXEC_SUCCESS : GE_PARSE_INTERNAL_FAIL;
}

int32_t GEXmlParserBase::ParseProdDoc()
{
    if (xmlProdDocument_ == nullptr) {
        GE_LOGE("GEXmlParserBase parse prod doc fail, document is null");
        return GE_PARSE_PROD_FILE_LOAD_FAIL;
    }

    xmlNode *root = xmlDocGetRootElement(xmlProdDocument_);
    if (root == nullptr) {
        GE_LOGE("GEXmlParserBase parse prod doc fail, root is null");
        return GE_PARSE_GET_ROOT_FAIL;
    }

    return ParseInternal(*root) ? GE_PARSE_EXEC_SUCCESS : GE_PARSE_INTERNAL_FAIL;
}

bool GEXmlParserBase::ParseInternal(xmlNode &node)
{
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        return true;
    }

    currNode = currNode->xmlChildrenNode;
    GEFeatureParamMapType featureMap;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (ParseFeatureParam(featureMap, *currNode) != GE_PARSE_EXEC_SUCCESS) {
            GE_LOGD("GEXmlParserBase parse internal fail");
            return false;
        }
    }
    return true;
}

bool GEXmlParserBase::ParseFeatureSwitch(std::string val)
{
    return val == "1" || val == "true" || val == "TRUE";
}

std::string GEXmlParserBase::ExtractPropertyValue(const std::string &propName, xmlNode &node)
{
    GE_LOGD("XMLParserBase extracting value : %{public}s", propName.c_str());
    std::string propValue = "";
    xmlChar *tempValue = nullptr;

    if (xmlHasProp(&node, reinterpret_cast<const xmlChar*>(propName.c_str()))) {
        tempValue = xmlGetProp(&node, reinterpret_cast<const xmlChar*>(propName.c_str()));
    }

    if (tempValue != nullptr) {
        GE_LOGD("XMLParserBase not a empty tempValue");
        propValue = reinterpret_cast<const char*>(tempValue);
        xmlFree(tempValue);
        tempValue = nullptr;
    }

    return propValue;
}

int32_t GEXmlParserBase::GetXmlNodeAsInt(xmlNode &node)
{
    if (node.name == nullptr) {
        return GE_PARSE_XML_UNDEFINED;
    }

    if (!xmlStrcmp(node.name, reinterpret_cast<const xmlChar*>("FeatureSwitch"))) {
        return GE_PARSE_XML_FEATURE_SWITCH;
    }
    if (!xmlStrcmp(node.name, reinterpret_cast<const xmlChar*>("FeatureSingleParam"))) {
        return GE_PARSE_XML_FEATURE_SINGLEPARAM;
    }
    if (!xmlStrcmp(node.name, reinterpret_cast<const xmlChar*>("FeatureMultiParam"))) {
        return GE_PARSE_XML_FEATURE_MULTIPARAM;
    }
    GE_LOGD("XMLParserBase failed to identify a xml node : %{public}s", node.name);
    return GE_PARSE_XML_UNDEFINED;
}

bool GEXmlParserBase::IsNumber(const std::string& str)
{
    if (str.length() == 0 || str.length() > XML_STRING_MAX_LENGTH) {
        return false;
    }
    auto number = static_cast<uint32_t>(std::count_if(str.begin(), str.end(), [](unsigned char c) {
        return std::isdigit(c);
    }));
    return number == str.length() || (str.compare(0, 1, "-") == 0 && number == str.length() - 1);
}

} // namespace Rosen
} // namespace OHOS