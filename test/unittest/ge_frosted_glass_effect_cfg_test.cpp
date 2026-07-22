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

#include <filesystem>
#include <gtest/gtest.h>
#include <cstring>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <cstdlib>

#include "ge_frosted_glass_effect_cfg.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEFrostedGlassEffectCfgTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    bool CheckFileGraphicConfig();
    void SetUp() override;
    void TearDown() override;
};

void GEFrostedGlassEffectCfgTest::SetUpTestCase(void)
{
    xmlInitParser();
}

void GEFrostedGlassEffectCfgTest::TearDownTestCase(void)
{
    xmlCleanupParser();
}

bool GEFrostedGlassEffectCfgTest::CheckFileGraphicConfig()
{
    std::string pathProd = "/sys_prod/etc/graphic/graphic_config.xml";
    std::string pathSys = "/system/variant/phone/base/etc/graphic/graphic_config.xml";
    bool result = (std::filesystem::exists(pathProd) || std::filesystem::exists(pathSys));
    GTEST_LOG_(INFO) << "GEFrostedGlassEffectCfgTest::CheckFileGraphicConfig " << result;
    return result;
}

void GEFrostedGlassEffectCfgTest::SetUp() {}
void GEFrostedGlassEffectCfgTest::TearDown() {}

HWTEST_F(GEFrostedGlassEffectCfgTest, GetInstance_001, TestSize.Level1)
{
    auto& instance1 = GEFrostedGlassEffectCfg::GetInstance();
    auto& instance2 = GEFrostedGlassEffectCfg::GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

HWTEST_F(GEFrostedGlassEffectCfgTest, GetIsDisableAntiAliasCode_001, TestSize.Level1)
{
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    instance.SetDisableAntiAliasCode(false);
    EXPECT_FALSE(instance.GetIsDisableAntiAliasCode());
}

HWTEST_F(GEFrostedGlassEffectCfgTest, GetIsDisableAntiAliasCode_002, TestSize.Level1)
{
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    instance.SetDisableAntiAliasCode(true);
    EXPECT_TRUE(instance.GetIsDisableAntiAliasCode());
}

HWTEST_F(GEFrostedGlassEffectCfgTest, SetDisableAntiAliasCode_001, TestSize.Level1)
{
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    instance.SetDisableAntiAliasCode(true);
    EXPECT_TRUE(instance.GetIsDisableAntiAliasCode());

    instance.SetDisableAntiAliasCode(false);
    EXPECT_FALSE(instance.GetIsDisableAntiAliasCode());
}

HWTEST_F(GEFrostedGlassEffectCfgTest, Init_001, TestSize.Level1)
{
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    bool result = instance.Init();
    EXPECT_TRUE(result);
}

HWTEST_F(GEFrostedGlassEffectCfgTest, GraphicConfigPath_001, TestSize.Level1)
{
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    EXPECT_EQ(instance.graphicConfigPath_, "etc/graphic/graphic_config.xml");
}

HWTEST_F(GEFrostedGlassEffectCfgTest, ParseFeatureParam_001, TestSize.Level1)
{
    if (!CheckFileGraphicConfig()) {
        return;
    }
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    const char* xmlContent = "<root name=\"OtherName\"><child/></root>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    GEFeatureParamMapType featureMap;
    int32_t result = instance.ParseFeatureParam(featureMap, *root);
    EXPECT_EQ(result, GE_PARSE_GET_CHILD_FAIL);

    xmlFreeDoc(doc);
}

HWTEST_F(GEFrostedGlassEffectCfgTest, ParseFeatureParam_002, TestSize.Level1)
{
    if (!CheckFileGraphicConfig()) {
        return;
    }
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    const char* xmlContent = "<root name=\"FrostedGlassEffectCfg\"><child/></root>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    GEFeatureParamMapType featureMap;
    int32_t result = instance.ParseFeatureParam(featureMap, *root);
    EXPECT_EQ(result, GE_PARSE_EXEC_SUCCESS);

    xmlFreeDoc(doc);
}

HWTEST_F(GEFrostedGlassEffectCfgTest, ParseFeatureParam_003, TestSize.Level1)
{
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    const char* xmlContent = "<root name=\"FrostedGlassEffectCfg\"/>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    GEFeatureParamMapType featureMap;
    int32_t result = instance.ParseFeatureParam(featureMap, *root);
    EXPECT_EQ(result, GE_PARSE_GET_CHILD_FAIL);

    xmlFreeDoc(doc);
}

HWTEST_F(GEFrostedGlassEffectCfgTest, ParseFeatureParam_004, TestSize.Level1)
{
    if (!CheckFileGraphicConfig()) {
        return;
    }
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    const char* xmlContent =
        "<root name=\"FrostedGlassEffectCfg\"><child name=\"EnableAntiAliasCode\" value=\"1\"/></root>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    instance.SetDisableAntiAliasCode(false);
    GEFeatureParamMapType featureMap;
    int32_t result = instance.ParseFeatureParam(featureMap, *root);
    EXPECT_EQ(result, GE_PARSE_EXEC_SUCCESS);

    xmlFreeDoc(doc);
}

HWTEST_F(GEFrostedGlassEffectCfgTest, ParseFeatureParam_005, TestSize.Level1)
{
    if (!CheckFileGraphicConfig()) {
        return;
    }
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    const char* xmlContent = "<root name=\"FrostedGlassEffectCfg\">text<!-- comment --><child/></root>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    GEFeatureParamMapType featureMap;
    int32_t result = instance.ParseFeatureParam(featureMap, *root);
    EXPECT_EQ(result, GE_PARSE_EXEC_SUCCESS);

    xmlFreeDoc(doc);
}

HWTEST_F(GEFrostedGlassEffectCfgTest, ParseFrostedGlassInternal_001, TestSize.Level1)
{
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    const char* xmlContent = "<FeatureSwitch name=\"EnableAntiAliasCode\" value=\"1\"/>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    instance.SetDisableAntiAliasCode(false);
    int32_t result = instance.ParseFrostedGlassInternal(*root);
    EXPECT_EQ(result, GE_PARSE_EXEC_SUCCESS);
    EXPECT_FALSE(instance.GetIsDisableAntiAliasCode());

    xmlFreeDoc(doc);
}

HWTEST_F(GEFrostedGlassEffectCfgTest, ParseFrostedGlassInternal_002, TestSize.Level1)
{
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    const char* xmlContent = "<FeatureSwitch name=\"EnableAntiAliasCode\" value=\"0\"/>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    instance.SetDisableAntiAliasCode(true);
    int32_t result = instance.ParseFrostedGlassInternal(*root);
    EXPECT_EQ(result, GE_PARSE_EXEC_SUCCESS);
    EXPECT_TRUE(instance.GetIsDisableAntiAliasCode());

    xmlFreeDoc(doc);
}

HWTEST_F(GEFrostedGlassEffectCfgTest, ParseFrostedGlassInternal_003, TestSize.Level1)
{
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    const char* xmlContent = "<FeatureSwitch name=\"OtherName\" value=\"1\"/>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    instance.SetDisableAntiAliasCode(false);
    int32_t result = instance.ParseFrostedGlassInternal(*root);
    EXPECT_EQ(result, GE_PARSE_EXEC_SUCCESS);
    EXPECT_FALSE(instance.GetIsDisableAntiAliasCode());

    xmlFreeDoc(doc);
}

HWTEST_F(GEFrostedGlassEffectCfgTest, ParseFrostedGlassInternal_004, TestSize.Level1)
{
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    const char* xmlContent = "<FeatureSingleParam name=\"SomeParam\" value=\"test\"/>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    instance.SetDisableAntiAliasCode(false);
    int32_t result = instance.ParseFrostedGlassInternal(*root);
    EXPECT_EQ(result, GE_PARSE_EXEC_SUCCESS);
    EXPECT_FALSE(instance.GetIsDisableAntiAliasCode());

    xmlFreeDoc(doc);
}

HWTEST_F(GEFrostedGlassEffectCfgTest, FeatureParamParseEntry_001, TestSize.Level1)
{
    if (!CheckFileGraphicConfig()) {
        return;
    }
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    bool result = instance.FeatureParamParseEntry();
    EXPECT_TRUE(result);
}

HWTEST_F(GEFrostedGlassEffectCfgTest, GetIsDisableAntiAliasCode_Static_001, TestSize.Level1)
{
    GEFrostedGlassEffectCfg::isDisableAntiAliasCode_ = true;
    EXPECT_TRUE(GEFrostedGlassEffectCfg::isDisableAntiAliasCode_);

    GEFrostedGlassEffectCfg::isDisableAntiAliasCode_ = false;
    EXPECT_FALSE(GEFrostedGlassEffectCfg::isDisableAntiAliasCode_);
}

HWTEST_F(GEFrostedGlassEffectCfgTest, ParseFeatureParam_MultipleChildren_001, TestSize.Level1)
{
    if (!CheckFileGraphicConfig()) {
        return;
    }
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    const char* xmlContent = "<root name=\"FrostedGlassEffectCfg\">"
        "<FeatureSwitch name=\"EnableAntiAliasCode\" value=\"true\"/>"
        "<FeatureSwitch name=\"OtherFeature\" value=\"1\"/>"
        "</root>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    instance.SetDisableAntiAliasCode(false);
    GEFeatureParamMapType featureMap;
    int32_t result = instance.ParseFeatureParam(featureMap, *root);
    EXPECT_EQ(result, GE_PARSE_EXEC_SUCCESS);

    xmlFreeDoc(doc);
}

HWTEST_F(GEFrostedGlassEffectCfgTest, ParseFeatureParam_ValueTrue_001, TestSize.Level1)
{
    if (!CheckFileGraphicConfig()) {
        return;
    }
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    const char* xmlContent = "<root name=\"FrostedGlassEffectCfg\">"
        "<FeatureSwitch name=\"EnableAntiAliasCode\" value=\"true\"/>"
        "</root>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    instance.SetDisableAntiAliasCode(false);
    GEFeatureParamMapType featureMap;
    int32_t result = instance.ParseFeatureParam(featureMap, *root);
    EXPECT_EQ(result, GE_PARSE_EXEC_SUCCESS);

    xmlFreeDoc(doc);
}

HWTEST_F(GEFrostedGlassEffectCfgTest, ParseFeatureParam_ValueTRUE_001, TestSize.Level1)
{
    if (!CheckFileGraphicConfig()) {
        return;
    }
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    const char* xmlContent = "<root name=\"FrostedGlassEffectCfg\">"
        "<FeatureSwitch name=\"EnableAntiAliasCode\" value=\"TRUE\"/>"
        "</root>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    instance.SetDisableAntiAliasCode(false);
    GEFeatureParamMapType featureMap;
    int32_t result = instance.ParseFeatureParam(featureMap, *root);
    EXPECT_EQ(result, GE_PARSE_EXEC_SUCCESS);

    xmlFreeDoc(doc);
}

HWTEST_F(GEFrostedGlassEffectCfgTest, ParseFrostedGlassInternal_ValueFalse_001, TestSize.Level1)
{
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    const char* xmlContent = "<FeatureSwitch name=\"EnableAntiAliasCode\" value=\"false\"/>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    instance.SetDisableAntiAliasCode(true);
    int32_t result = instance.ParseFrostedGlassInternal(*root);
    EXPECT_EQ(result, GE_PARSE_EXEC_SUCCESS);
    EXPECT_TRUE(instance.GetIsDisableAntiAliasCode());

    xmlFreeDoc(doc);
}

HWTEST_F(GEFrostedGlassEffectCfgTest, ParseFeatureParam_BreakWithChildren_001, TestSize.Level1)
{
    if (!CheckFileGraphicConfig()) {
        return;
    }
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    const char* xmlContent = "<root name=\"OtherName\">"
        "<FeatureSwitch name=\"EnableAntiAliasCode\" value=\"1\"/>"
        "</root>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    instance.SetDisableAntiAliasCode(false);
    GEFeatureParamMapType featureMap;
    int32_t result = instance.ParseFeatureParam(featureMap, *root);
    EXPECT_EQ(result, GE_PARSE_GET_CHILD_FAIL);
    EXPECT_FALSE(instance.GetIsDisableAntiAliasCode());

    xmlFreeDoc(doc);
}

HWTEST_F(GEFrostedGlassEffectCfgTest, ParseFeatureParam_BreakWithMultipleChildren_001, TestSize.Level1)
{
    if (!CheckFileGraphicConfig()) {
        return;
    }
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    const char* xmlContent = "<root name=\"OtherName\">"
        "<FeatureSwitch name=\"EnableAntiAliasCode\" value=\"true\"/>"
        "<FeatureSwitch name=\"OtherFeature\" value=\"1\"/>"
        "</root>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    instance.SetDisableAntiAliasCode(false);
    GEFeatureParamMapType featureMap;
    int32_t result = instance.ParseFeatureParam(featureMap, *root);
    EXPECT_EQ(result, GE_PARSE_GET_CHILD_FAIL);
    EXPECT_FALSE(instance.GetIsDisableAntiAliasCode());

    xmlFreeDoc(doc);
}

HWTEST_F(GEFrostedGlassEffectCfgTest, ParseFeatureParam_BreakWithTextAndComment_001, TestSize.Level1)
{
    if (!CheckFileGraphicConfig()) {
        return;
    }
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    const char* xmlContent = "<root name=\"OtherName\">text<!-- comment -->"
        "<FeatureSwitch name=\"EnableAntiAliasCode\" value=\"TRUE\"/>"
        "</root>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    instance.SetDisableAntiAliasCode(false);
    GEFeatureParamMapType featureMap;
    int32_t result = instance.ParseFeatureParam(featureMap, *root);
    EXPECT_EQ(result, GE_PARSE_GET_CHILD_FAIL);
    EXPECT_FALSE(instance.GetIsDisableAntiAliasCode());

    xmlFreeDoc(doc);
}

HWTEST_F(GEFrostedGlassEffectCfgTest, ParseFeatureParam_BreakNoChildren_001, TestSize.Level1)
{
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    const char* xmlContent = "<root name=\"OtherName\"/>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    GEFeatureParamMapType featureMap;
    int32_t result = instance.ParseFeatureParam(featureMap, *root);
    EXPECT_EQ(result, GE_PARSE_GET_CHILD_FAIL);

    xmlFreeDoc(doc);
}

HWTEST_F(GEFrostedGlassEffectCfgTest, ParseFeatureParam_BreakWithFeatureSingleParam_001, TestSize.Level1)
{
    if (!CheckFileGraphicConfig()) {
        return;
    }
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    const char* xmlContent = "<root name=\"OtherName\">"
        "<FeatureSingleParam name=\"SomeParam\" value=\"test\"/>"
        "</root>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    instance.SetDisableAntiAliasCode(false);
    GEFeatureParamMapType featureMap;
    int32_t result = instance.ParseFeatureParam(featureMap, *root);
    EXPECT_EQ(result, GE_PARSE_GET_CHILD_FAIL);
    EXPECT_FALSE(instance.GetIsDisableAntiAliasCode());

    xmlFreeDoc(doc);
}

HWTEST_F(GEFrostedGlassEffectCfgTest, ParseFeatureParam_BreakWithValueFalse_001, TestSize.Level1)
{
    if (!CheckFileGraphicConfig()) {
        return;
    }
    auto& instance = GEFrostedGlassEffectCfg::GetInstance();
    const char* xmlContent = "<root name=\"OtherName\">"
        "<FeatureSwitch name=\"EnableAntiAliasCode\" value=\"false\"/>"
        "</root>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    instance.SetDisableAntiAliasCode(true);
    GEFeatureParamMapType featureMap;
    int32_t result = instance.ParseFeatureParam(featureMap, *root);
    EXPECT_EQ(result, GE_PARSE_GET_CHILD_FAIL);
    EXPECT_TRUE(instance.GetIsDisableAntiAliasCode());

    xmlFreeDoc(doc);
}

} // namespace Rosen
} // namespace OHOS