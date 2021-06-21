//
// Unit Tests: XML
//
// Description:
//
// ================
// Test definitions
// =================
#include "XML_tests.hpp"
// =======================
// JSON class namespace
// =======================
using namespace H4;
// ==========
// Test cases
// ==========
TEST_CASE("Parse XML with defined namespaces.", "[XML][Parse][Namespace]")
{
  std::string xmlString;
  SECTION("A root document and two namespaces defined in the child two table elements.", "[XML][Parse][Namespace]")
  {
    xmlString = "<root>\n"
                "<h:table xmlns:h=\"http://www.w3.org/TR/html4/\">\n"
                "<h:tr>\n"
                "<h:td>Apples</h:td>\n"
                "<h:td>Bananas</h:td>\n"
                "</h:tr>\n"
                "</h:table>\n"
                "<f:table xmlns:f=\"https://www.w3schools.com/furniture\">\n"
                "<f:name>African Coffee Table</f:name>\n"
                "<f:width>80</f:width>\n"
                "<f:length>120</f:length>\n"
                "</f:table>\n"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(xml.m_prolog[0][0].name == "h:table");
    REQUIRE(xml.m_prolog[0][0].getNameSpaceList().size() == 1);
    REQUIRE(xml.m_prolog[0][0].getNameSpace("h").value.parsed == "http://www.w3.org/TR/html4/");
    REQUIRE(xml.m_prolog[0][1].getNameSpaceList().size() == 1);
    REQUIRE(xml.m_prolog[0][1].name == "f:table");
    REQUIRE(xml.m_prolog[0][1].getNameSpace("f").value.parsed == "https://www.w3schools.com/furniture");
  }
  SECTION("A root document and two namespaces defined in the root element.", "[XML][Parse][Namespace]")
  {
    xmlString = "<root xmlns:h=\"http://www.w3.org/TR/html4/\" xmlns:f=\"https://www.w3schools.com/furniture\">\n"
                "<h:table>\n"
                "<h:tr>\n"
                "<h:td>Apples</h:td>\n"
                "<h:td>Bananas</h:td>\n"
                "</h:tr>\n"
                "</h:table>\n"
                "<f:table>\n"
                "<f:name>African Coffee Table</f:name>\n"
                "<f:width>80</f:width>\n"
                "<f:length>120</f:length>\n"
                "</f:table>\n"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(xml.m_prolog[0][0].name == "h:table");
    REQUIRE(xml.m_prolog[0][0].getNameSpaceList().size() == 2);
    REQUIRE(xml.m_prolog[0][0].getNameSpace("h").value.parsed == "http://www.w3.org/TR/html4/");
    REQUIRE(xml.m_prolog[0][0].getNameSpace("f").value.parsed == "https://www.w3schools.com/furniture");
    REQUIRE(xml.m_prolog[0][1].name == "f:table");
    REQUIRE(xml.m_prolog[0][1].getNameSpaceList().size() == 2);
    REQUIRE(xml.m_prolog[0][1].getNameSpace("h").value.parsed == "http://www.w3.org/TR/html4/");
    REQUIRE(xml.m_prolog[0][1].getNameSpace("f").value.parsed == "https://www.w3schools.com/furniture");
  }
  SECTION("A root document and two namespaces defined in the root element and non-existant namespace g for one of tables.", "[XML][Parse][Namespace]")
  {
    xmlString = "<root xmlns:h=\"http://www.w3.org/TR/html4/\" xmlns:f=\"https://www.w3schools.com/furniture\">\n"
                "<h:table><h:tr><h:td>Apples</h:td><h:td>Bananas</h:td></h:tr></h:table><g:table>\n"
                "<g:name>African Coffee Table</g:name><g:width>80</g:width>\n"
                "<g:length>120</g:length></g:table></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 3 Column: 44] Namespace used but not defined.");
  }
  SECTION("A root documement with a default namespace", "[XML][Parse][Namespace]")
  {
    xmlString = "<table xmlns=\"http://www.w3.org/TR/html4/\"><tr><td>Apples</td><td>Bananas</td></tr></table>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(xml.m_prolog[0][0].name == "tr");
    REQUIRE(xml.m_prolog[0][0].getNameSpaceList().size() == 1);
    REQUIRE(xml.m_prolog[0][0].getNameSpace(":").value.parsed == "http://www.w3.org/TR/html4/");
  }
  SECTION("A root document and two namespaces (the same name) defined in the root element.", "[XML][Parse][Namespace]")
  {
    xmlString = "<root xmlns:f=\"http://www.w3.org/TR/html4/\" xmlns:f=\"https://www.w3schools.com/furniture\">\n"
                "<f:table><f:tr><f:td>Apples</f:td><f:td>Bananas</f:td></f:tr></f:table><f:table>\n"
                "<f:name>African Coffee Table</f:name><f:width>80</f:width>\n"
                "<f:length>120</f:length></f:table></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 1 Column: 94] Attribute defined more than once within start tag.");
  }
  SECTION("A root document defining one namespae tha is overridden by a child", "[XML][Parse][Namespace]")
  {
    xmlString = "<root xmlns:h=\"http://www.w3.org/TR/html4/\" xmlns:f=\"https://www.w3schools.com/furniture\">\n"
                "<h:table><h:tr><h:td>Apples</h:td><h:td>Bananas</h:td></h:tr></h:table>\n"
                "<f:table xmlns:f=\"https://www.w3schools.com/furniture\">\n"
                "<f:name>African Coffee Table</f:name><f:width>80</f:width>\n"
                "<f:length>120</f:length></f:table></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
}