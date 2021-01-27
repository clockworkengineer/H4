//
// Unit Tests: JSON
//
// Description: ISource, IDestination and JNode index/reference
// unit tests for JSON class using the Catch2 test framework.
//
// ================
// Test definitions
// =================
#include "XML_tests.hpp"
// =======================
// JSON class namespace
// =======================
using namespace H4;
// ==========================
// Unit test helper functions
// ==========================
// ==========
// Test cases
// ==========
TEST_CASE("Use XML object to parse XML declaration", "[XML]")
{
  XML xml;
  SECTION("Parse basic XML declaration. ", "[XML]")
  {
    XNodeRoot xNodeRoot = xml.parse(kXMLDeclaration1);
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "no");
  }
  SECTION("Parse version 1.0 encoding == UTF-16 standalone == yes XML declaration. ", "[XML]")
  {
    XNodeRoot xNodeRoot = xml.parse(kXMLDeclaration2);
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-16");
    REQUIRE(xNodeRoot.standalone == "yes");
  }
  SECTION("Parse version 1.1 encoding == UTF8 standalone == yes XML declaration. ", "[XML]")
  {
    XNodeRoot xNodeRoot = xml.parse(kXMLDeclaration3);
    REQUIRE(xNodeRoot.version == "1.1");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "yes");
  }
  SECTION("Parse empty XML declaration. ", "[XML]")
  {
    XNodeRoot xNodeRoot = xml.parse(kXMLDeclaration4);
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "no");
  }
  SECTION("Parse empty XML declaration no end tag ", "[XML]")
  {
    XNodeRoot xNodeRoot;
    REQUIRE_THROWS_AS(xml.parse(kXMLDeclaration5), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(kXMLDeclaration5), "XML syntax error detected.");
  }
  SECTION("Parse wrongly ordered attributes in XML declaration. ", "[XML]")
  {
    REQUIRE_THROWS_AS(xml.parse(kXMLDeclaration6), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(kXMLDeclaration6), "XML syntax error detected.");
  }
}