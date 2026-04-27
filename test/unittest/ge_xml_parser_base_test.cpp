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

#include <gtest/gtest.h>
#include <cstring>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "ge_xml_parser_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEXmlParserBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEXmlParserBaseTest::SetUpTestCase(void)
{
    xmlInitParser();
}

void GEXmlParserBaseTest::TearDownTestCase(void)
{
    xmlCleanupParser();
}

void GEXmlParserBaseTest::SetUp() {}
void GEXmlParserBaseTest::TearDown() {}

HWTEST_F(GEXmlParserBaseTest, IsNumber_001, TestSize.Level1)
{
    EXPECT_FALSE(GEXmlParserBase::IsNumber(""));
}

HWTEST_F(GEXmlParserBaseTest, IsNumber_002, TestSize.Level1)
{
    std::string longStr(21, '1');
    EXPECT_FALSE(GEXmlParserBase::IsNumber(longStr));
}

HWTEST_F(GEXmlParserBaseTest, IsNumber_003, TestSize.Level1)
{
    EXPECT_TRUE(GEXmlParserBase::IsNumber("12345"));
    EXPECT_TRUE(GEXmlParserBase::IsNumber("0"));
    EXPECT_TRUE(GEXmlParserBase::IsNumber("99999999999999999999"));
}

HWTEST_F(GEXmlParserBaseTest, IsNumber_004, TestSize.Level1)
{
    EXPECT_TRUE(GEXmlParserBase::IsNumber("-12345"));
    EXPECT_TRUE(GEXmlParserBase::IsNumber("-1"));
    EXPECT_TRUE(GEXmlParserBase::IsNumber("-9999999999999999999"));
}

HWTEST_F(GEXmlParserBaseTest, IsNumber_005, TestSize.Level1)
{
    EXPECT_FALSE(GEXmlParserBase::IsNumber("abc"));
    EXPECT_FALSE(GEXmlParserBase::IsNumber("12a34"));
    EXPECT_FALSE(GEXmlParserBase::IsNumber("1.23"));
    EXPECT_FALSE(GEXmlParserBase::IsNumber("--123"));
    EXPECT_FALSE(GEXmlParserBase::IsNumber("123-"));
}

HWTEST_F(GEXmlParserBaseTest, ParseFeatureSwitch_001, TestSize.Level1)
{
    GEXmlParserBase parser;
    EXPECT_TRUE(parser.ParseFeatureSwitch("1"));
}

HWTEST_F(GEXmlParserBaseTest, ParseFeatureSwitch_002, TestSize.Level1)
{
    GEXmlParserBase parser;
    EXPECT_TRUE(parser.ParseFeatureSwitch("true"));
}

HWTEST_F(GEXmlParserBaseTest, ParseFeatureSwitch_003, TestSize.Level1)
{
    GEXmlParserBase parser;
    EXPECT_TRUE(parser.ParseFeatureSwitch("TRUE"));
}

HWTEST_F(GEXmlParserBaseTest, ParseFeatureSwitch_004, TestSize.Level1)
{
    GEXmlParserBase parser;
    EXPECT_FALSE(parser.ParseFeatureSwitch("0"));
    EXPECT_FALSE(parser.ParseFeatureSwitch("false"));
    EXPECT_FALSE(parser.ParseFeatureSwitch("False"));
    EXPECT_FALSE(parser.ParseFeatureSwitch("TRUE "));
    EXPECT_FALSE(parser.ParseFeatureSwitch(" true"));
    EXPECT_FALSE(parser.ParseFeatureSwitch(""));
    EXPECT_FALSE(parser.ParseFeatureSwitch("yes"));
}

HWTEST_F(GEXmlParserBaseTest, GetXmlNodeAsInt_001, TestSize.Level1)
{
    GEXmlParserBase parser;
    xmlNode node{};
    node.name = nullptr;

    EXPECT_EQ(parser.GetXmlNodeAsInt(node), GE_PARSE_XML_UNDEFINED);
}

HWTEST_F(GEXmlParserBaseTest, GetXmlNodeAsInt_002, TestSize.Level1)
{
    GEXmlParserBase parser;
    xmlNode node{};
    node.name = reinterpret_cast<const xmlChar*>("FeatureSwitch");

    EXPECT_EQ(parser.GetXmlNodeAsInt(node), GE_PARSE_XML_FEATURE_SWITCH);
}

HWTEST_F(GEXmlParserBaseTest, GetXmlNodeAsInt_003, TestSize.Level1)
{
    GEXmlParserBase parser;
    xmlNode node{};
    node.name = reinterpret_cast<const xmlChar*>("FeatureSingleParam");

    EXPECT_EQ(parser.GetXmlNodeAsInt(node), GE_PARSE_XML_FEATURE_SINGLEPARAM);
}

HWTEST_F(GEXmlParserBaseTest, GetXmlNodeAsInt_004, TestSize.Level1)
{
    GEXmlParserBase parser;
    xmlNode node{};
    node.name = reinterpret_cast<const xmlChar*>("FeatureMultiParam");

    EXPECT_EQ(parser.GetXmlNodeAsInt(node), GE_PARSE_XML_FEATURE_MULTIPARAM);
}

HWTEST_F(GEXmlParserBaseTest, GetXmlNodeAsInt_005, TestSize.Level1)
{
    GEXmlParserBase parser;
    xmlNode node{};
    node.name = reinterpret_cast<const xmlChar*>("UnknownNode");

    EXPECT_EQ(parser.GetXmlNodeAsInt(node), GE_PARSE_XML_UNDEFINED);
}

HWTEST_F(GEXmlParserBaseTest, ExtractPropertyValue_001, TestSize.Level1)
{
    GEXmlParserBase parser;
    const char* xmlContent = "<root><child/></root>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "noname.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);
    xmlNodePtr child = root->children;
    ASSERT_NE(child, nullptr);

    std::string result = parser.ExtractPropertyValue("nonexistent", *child);
    EXPECT_EQ(result, "");

    xmlFreeDoc(doc);
}

HWTEST_F(GEXmlParserBaseTest, ExtractPropertyValue_002, TestSize.Level1)
{
    GEXmlParserBase parser;
    const char* xmlContent = "<root><child name=\"testValue\"/></root>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "noname.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);
    xmlNodePtr child = root->children;
    ASSERT_NE(child, nullptr);

    std::string result = parser.ExtractPropertyValue("name", *child);
    EXPECT_EQ(result, "testValue");

    xmlFreeDoc(doc);
}

HWTEST_F(GEXmlParserBaseTest, ExtractPropertyValue_003, TestSize.Level1)
{
    GEXmlParserBase parser;
    const char* xmlContent = "<root><child name=\"\"/></root>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "noname.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);
    xmlNodePtr child = root->children;
    ASSERT_NE(child, nullptr);

    std::string result = parser.ExtractPropertyValue("name", *child);
    EXPECT_EQ(result, "");

    xmlFreeDoc(doc);
}

HWTEST_F(GEXmlParserBaseTest, Clear_001, TestSize.Level1)
{
    GEXmlParserBase parser;
    parser.Clear();
}

HWTEST_F(GEXmlParserBaseTest, Clear_002, TestSize.Level1)
{
    GEXmlParserBase parser;
    const char* xmlContent = "<root><child/></root>";

    xmlDocPtr sysDoc = xmlReadMemory(xmlContent, strlen(xmlContent), "sys.xml", nullptr, 0);
    xmlDocPtr prodDoc = xmlReadMemory(xmlContent, strlen(xmlContent), "prod.xml", nullptr, 0);

    parser.xmlSysDocument_ = sysDoc;
    parser.xmlProdDocument_ = prodDoc;

    EXPECT_NE(parser.xmlSysDocument_, nullptr);
    EXPECT_NE(parser.xmlProdDocument_, nullptr);

    parser.Clear();

    EXPECT_EQ(parser.xmlSysDocument_, nullptr);
    EXPECT_EQ(parser.xmlProdDocument_, nullptr);
}

HWTEST_F(GEXmlParserBaseTest, Destroy_001, TestSize.Level1)
{
    GEXmlParserBase parser;
    const char* xmlContent = "<root><child/></root>";

    xmlDocPtr sysDoc = xmlReadMemory(xmlContent, strlen(xmlContent), "sys.xml", nullptr, 0);
    xmlDocPtr prodDoc = xmlReadMemory(xmlContent, strlen(xmlContent), "prod.xml", nullptr, 0);

    parser.xmlSysDocument_ = sysDoc;
    parser.xmlProdDocument_ = prodDoc;

    parser.Destroy();

    EXPECT_EQ(parser.xmlSysDocument_, nullptr);
    EXPECT_EQ(parser.xmlProdDocument_, nullptr);
}

HWTEST_F(GEXmlParserBaseTest, LoadGraphicConfiguration_001, TestSize.Level1)
{
    GEXmlParserBase parser;
    int32_t result = parser.LoadGraphicConfiguration("nonexistent/path/to/config.xml");
    EXPECT_EQ(result, GE_PARSE_SYS_FILE_LOAD_FAIL);
}

HWTEST_F(GEXmlParserBaseTest, ParseSysDoc_001, TestSize.Level1)
{
    GEXmlParserBase parser;
    int32_t result = parser.ParseSysDoc();
    EXPECT_EQ(result, GE_PARSE_SYS_FILE_LOAD_FAIL);
}

HWTEST_F(GEXmlParserBaseTest, ParseSysDoc_002, TestSize.Level1)
{
    GEXmlParserBase parser;
    const char* xmlContent = "<root><child/></root>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    parser.xmlSysDocument_ = doc;

    int32_t result = parser.ParseSysDoc();
    EXPECT_EQ(result, GE_PARSE_EXEC_SUCCESS);
}

HWTEST_F(GEXmlParserBaseTest, ParseSysDoc_003, TestSize.Level1)
{
    GEXmlParserBase parser;
    xmlDocPtr doc = xmlNewDoc(reinterpret_cast<const xmlChar*>("1.0"));
    ASSERT_NE(doc, nullptr);

    parser.xmlSysDocument_ = doc;

    int32_t result = parser.ParseSysDoc();
    EXPECT_EQ(result, GE_PARSE_GET_ROOT_FAIL);

    xmlFreeDoc(doc);
    parser.xmlSysDocument_ = nullptr;
}

HWTEST_F(GEXmlParserBaseTest, ParseProdDoc_001, TestSize.Level1)
{
    GEXmlParserBase parser;
    int32_t result = parser.ParseProdDoc();
    EXPECT_EQ(result, GE_PARSE_PROD_FILE_LOAD_FAIL);
}

HWTEST_F(GEXmlParserBaseTest, ParseProdDoc_002, TestSize.Level1)
{
    GEXmlParserBase parser;
    const char* xmlContent = "<root><child/></root>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    parser.xmlProdDocument_ = doc;

    int32_t result = parser.ParseProdDoc();
    EXPECT_EQ(result, GE_PARSE_EXEC_SUCCESS);
}

HWTEST_F(GEXmlParserBaseTest, ParseProdDoc_003, TestSize.Level1)
{
    GEXmlParserBase parser;
    xmlDocPtr doc = xmlNewDoc(reinterpret_cast<const xmlChar*>("1.0"));
    ASSERT_NE(doc, nullptr);

    parser.xmlProdDocument_ = doc;

    int32_t result = parser.ParseProdDoc();
    EXPECT_EQ(result, GE_PARSE_GET_ROOT_FAIL);

    xmlFreeDoc(doc);
    parser.xmlProdDocument_ = nullptr;
}

HWTEST_F(GEXmlParserBaseTest, ParseInternal_001, TestSize.Level1)
{
    GEXmlParserBase parser;
    const char* xmlContent = "<root/>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    bool result = parser.ParseInternal(*root);
    EXPECT_TRUE(result);

    xmlFreeDoc(doc);
}

HWTEST_F(GEXmlParserBaseTest, ParseInternal_002, TestSize.Level1)
{
    GEXmlParserBase parser;
    const char* xmlContent = "<root><child1/><child2/><child3/></root>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    bool result = parser.ParseInternal(*root);
    EXPECT_TRUE(result);

    xmlFreeDoc(doc);
}

HWTEST_F(GEXmlParserBaseTest, ParseInternal_003, TestSize.Level1)
{
    GEXmlParserBase parser;
    const char* xmlContent = "<root>text<!-- comment --><child/></root>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    bool result = parser.ParseInternal(*root);
    EXPECT_TRUE(result);

    xmlFreeDoc(doc);
}

HWTEST_F(GEXmlParserBaseTest, LoadSysConfiguration_001, TestSize.Level1)
{
    GEXmlParserBase parser;
    int32_t result = parser.LoadSysConfiguration("nonexistent.xml");
    EXPECT_EQ(result, GE_PARSE_SYS_FILE_LOAD_FAIL);
    EXPECT_EQ(parser.xmlSysDocument_, nullptr);
}

HWTEST_F(GEXmlParserBaseTest, LoadProdConfiguration_001, TestSize.Level1)
{
    GEXmlParserBase parser;
    parser.LoadProdConfiguration("nonexistent.xml");
    EXPECT_EQ(parser.xmlProdDocument_, nullptr);
}

HWTEST_F(GEXmlParserBaseTest, Destructor_001, TestSize.Level1)
{
    const char* xmlContent = "<root><child/></root>";
    xmlDocPtr sysDoc = nullptr;
    xmlDocPtr prodDoc = nullptr;

    {
        GEXmlParserBase parser;
        sysDoc = xmlReadMemory(xmlContent, strlen(xmlContent), "sys.xml", nullptr, 0);
        prodDoc = xmlReadMemory(xmlContent, strlen(xmlContent), "prod.xml", nullptr, 0);

        parser.xmlSysDocument_ = sysDoc;
        parser.xmlProdDocument_ = prodDoc;
    }
}

HWTEST_F(GEXmlParserBaseTest, ParseFeatureParam_001, TestSize.Level1)
{
    GEXmlParserBase parser;
    const char* xmlContent = "<root><child/></root>";
    xmlDocPtr doc = xmlReadMemory(xmlContent, strlen(xmlContent), "test.xml", nullptr, 0);
    ASSERT_NE(doc, nullptr);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    ASSERT_NE(root, nullptr);

    GEFeatureParamMapType featureMap;
    int32_t result = parser.ParseFeatureParam(featureMap, *root);
    EXPECT_EQ(result, GE_PARSE_EXEC_SUCCESS);

    xmlFreeDoc(doc);
}

HWTEST_F(GEXmlParserBaseTest, SysPaths_001, TestSize.Level1)
{
    GEXmlParserBase parser;
    EXPECT_EQ(parser.sysPaths_.size(), 7u);
}

HWTEST_F(GEXmlParserBaseTest, ProdPath_001, TestSize.Level1)
{
    GEXmlParserBase parser;
    EXPECT_EQ(parser.prodPath_, "/sys_prod/");
}

HWTEST_F(GEXmlParserBaseTest, IsNumber_Boundary_001, TestSize.Level1)
{
    EXPECT_TRUE(GEXmlParserBase::IsNumber("0"));
    EXPECT_TRUE(GEXmlParserBase::IsNumber("-0"));
    EXPECT_FALSE(GEXmlParserBase::IsNumber("+123"));
    EXPECT_FALSE(GEXmlParserBase::IsNumber("  123"));
    EXPECT_FALSE(GEXmlParserBase::IsNumber("123  "));
    EXPECT_FALSE(GEXmlParserBase::IsNumber("12 34"));
}

HWTEST_F(GEXmlParserBaseTest, IsNumber_Boundary_002, TestSize.Level1)
{
    std::string maxLenStr(20, '9');
    EXPECT_TRUE(GEXmlParserBase::IsNumber(maxLenStr));

    std::string maxLenNegStr(19, '9');
    maxLenNegStr = "-" + maxLenNegStr;
    EXPECT_TRUE(GEXmlParserBase::IsNumber(maxLenNegStr));
}

} // namespace Rosen
} // namespace OHOS